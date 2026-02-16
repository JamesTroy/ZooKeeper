#include "BTTask_AnimalSocialize.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"
#include "AIController.h"
#include "EngineUtils.h"

UBTTask_AnimalSocialize::UBTTask_AnimalSocialize()
{
	NodeName           = TEXT("Animal Socialize");
	bNotifyTick        = true;
	SocializeAmount    = 0.3f;
	SearchRadius       = 1500.0f;
	InteractionDistance = 200.0f;
	SocializeDuration  = 3.0f;
	CurrentPhase       = ESocializePhase::MovingToPartner;
	ElapsedTime        = 0.0f;
}

EBTNodeResult::Type UBTTask_AnimalSocialize::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                          uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AAnimalBase* Animal = Cast<AAnimalBase>(AIController->GetPawn());
	if (!Animal || !Animal->NeedsComponent)
	{
		return EBTNodeResult::Failed;
	}

	// Find the nearest animal of the same species.
	AAnimalBase* NearestPartner = nullptr;
	float NearestDistSq = SearchRadius * SearchRadius;
	const FVector MyLocation = Animal->GetActorLocation();

	UWorld* World = Animal->GetWorld();
	if (!World)
	{
		return EBTNodeResult::Failed;
	}

	for (TActorIterator<AAnimalBase> It(World); It; ++It)
	{
		AAnimalBase* Other = *It;
		if (Other == Animal)
		{
			continue;
		}

		// Must be the same species.
		if (Other->SpeciesID != Animal->SpeciesID)
		{
			continue;
		}

		// Prefer animals in the same enclosure.
		if (Animal->CurrentEnclosure && Other->CurrentEnclosure != Animal->CurrentEnclosure)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(MyLocation, Other->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestPartner = Other;
		}
	}

	if (!NearestPartner)
	{
		UE_LOG(LogZooKeeper, Verbose,
		       TEXT("BTTask_AnimalSocialize: '%s' found no same-species partner."),
		       *Animal->AnimalName);
		return EBTNodeResult::Failed;
	}

	PartnerActor = NearestPartner;
	CurrentPhase = ESocializePhase::MovingToPartner;
	ElapsedTime  = 0.0f;

	AIController->MoveToActor(NearestPartner, InteractionDistance);

	UE_LOG(LogZooKeeper, Verbose,
	       TEXT("BTTask_AnimalSocialize: '%s' moving toward '%s'."),
	       *Animal->AnimalName, *NearestPartner->AnimalName);

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalSocialize::TickTask(UBehaviorTreeComponent& OwnerComp,
                                        uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAnimalBase* Animal = Cast<AAnimalBase>(AIController->GetPawn());
	if (!Animal)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	switch (CurrentPhase)
	{
	case ESocializePhase::MovingToPartner:
	{
		// Check if the partner is still valid.
		if (!PartnerActor.IsValid())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		const EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
		if (MoveStatus != EPathFollowingStatus::Moving)
		{
			// Arrived or failed to reach partner -- start socializing.
			CurrentPhase = ESocializePhase::Socializing;
			ElapsedTime  = 0.0f;

			// Apply social boost to both animals.
			if (Animal->NeedsComponent)
			{
				Animal->NeedsComponent->Socialize(SocializeAmount);
			}

			AAnimalBase* Partner = Cast<AAnimalBase>(PartnerActor.Get());
			if (Partner && Partner->NeedsComponent)
			{
				Partner->NeedsComponent->Socialize(SocializeAmount);
			}

			UE_LOG(LogZooKeeper, Verbose,
			       TEXT("BTTask_AnimalSocialize: '%s' socializing with partner."),
			       *Animal->AnimalName);
		}
		break;
	}

	case ESocializePhase::Socializing:
	{
		ElapsedTime += DeltaSeconds;
		if (ElapsedTime >= SocializeDuration)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		break;
	}
	}
}

FString UBTTask_AnimalSocialize::GetStaticDescription() const
{
	return FString::Printf(TEXT("Socialize (+%.0f%% social to both)"), SocializeAmount * 100.0f);
}
