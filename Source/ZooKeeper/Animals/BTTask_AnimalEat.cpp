#include "BTTask_AnimalEat.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "Buildings/EnclosureActor.h"
#include "Buildings/FeederActor.h"
#include "ZooKeeper.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_AnimalEat::UBTTask_AnimalEat()
{
	NodeName    = TEXT("Animal Eat");
	bNotifyTick = true;
	FeedAmount  = 0.3f;
	EatDuration = 2.0f;
	ElapsedTime = 0.0f;
}

EBTNodeResult::Type UBTTask_AnimalEat::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
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

	// Try to find a feeder in the animal's enclosure.
	AEnclosureActor* Enclosure = Cast<AEnclosureActor>(Animal->CurrentEnclosure);
	AFeederActor* Feeder = nullptr;

	if (Enclosure)
	{
		// Search for a non-empty feeder tagged or within the enclosure area.
		TArray<AActor*> Feeders;
		UGameplayStatics::GetAllActorsOfClass(Animal->GetWorld(), AFeederActor::StaticClass(), Feeders);

		float NearestDistSq = TNumericLimits<float>::Max();
		const FVector EnclosureLoc = Enclosure->GetActorLocation();

		for (AActor* Actor : Feeders)
		{
			AFeederActor* CandidateFeeder = Cast<AFeederActor>(Actor);
			if (!CandidateFeeder || CandidateFeeder->IsEmpty())
			{
				continue;
			}

			const float DistSq = FVector::DistSquared(EnclosureLoc, CandidateFeeder->GetActorLocation());
			if (DistSq < NearestDistSq)
			{
				NearestDistSq = DistSq;
				Feeder = CandidateFeeder;
			}
		}
	}

	if (Feeder)
	{
		// Move to feeder and consume food from it.
		AIController->MoveToActor(Feeder, 100.0f);

		if (Feeder->ConsumeFood())
		{
			Animal->NeedsComponent->FeedAnimal(Feeder->HungerRestorePerUse);
			UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalEat: '%s' ate from feeder [%s]."),
			       *Animal->AnimalName, *Feeder->GetName());
		}
	}
	else
	{
		// Fallback: no feeder available, feed a reduced amount directly (foraging).
		Animal->NeedsComponent->FeedAnimal(FeedAmount * 0.5f);
		UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalEat: '%s' foraged (no feeder available)."),
		       *Animal->AnimalName);
	}

	ElapsedTime = 0.0f;
	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalEat::TickTask(UBehaviorTreeComponent& OwnerComp,
                                  uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime >= EatDuration)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalEat: finished eating."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_AnimalEat::GetStaticDescription() const
{
	return FString::Printf(TEXT("Eat for %.1fs (restore %.0f%% hunger)"), EatDuration, FeedAmount * 100.0f);
}
