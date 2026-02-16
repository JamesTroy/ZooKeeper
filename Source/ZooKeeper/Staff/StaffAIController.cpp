#include "StaffAIController.h"
#include "StaffCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Buildings/EnclosureActor.h"
#include "Buildings/FeederActor.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZooKeeper.h"

AStaffAIController::AStaffAIController()
{
	StaffBehaviorTree = nullptr;
}

void AStaffAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (StaffBehaviorTree)
	{
		RunBehaviorTree(StaffBehaviorTree);

		AStaffCharacter* StaffCharacter = Cast<AStaffCharacter>(InPawn);
		if (StaffCharacter)
		{
			UE_LOG(LogZooKeeper, Log, TEXT("StaffAIController [%s] started behavior tree for staff [%s] (%s)."),
				*GetName(), *StaffCharacter->StaffName, *UEnum::GetValueAsString(StaffCharacter->StaffType));
		}
		else
		{
			UE_LOG(LogZooKeeper, Log, TEXT("StaffAIController [%s] started behavior tree for pawn [%s]."),
				*GetName(), *InPawn->GetName());
		}
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffAIController [%s] has no StaffBehaviorTree assigned."),
			*GetName());
	}
}

void AStaffAIController::OnUnPossess()
{
	UBrainComponent* BrainComp = GetBrainComponent();
	if (BrainComp)
	{
		BrainComp->StopLogic(TEXT("Unpossessed"));
	}

	Super::OnUnPossess();
}

FVector AStaffAIController::FindTaskInEnclosure() const
{
	AStaffCharacter* StaffCharacter = Cast<AStaffCharacter>(GetPawn());
	if (!StaffCharacter)
	{
		return FVector::ZeroVector;
	}

	AEnclosureActor* Enclosure = StaffCharacter->AssignedEnclosure;
	if (!Enclosure)
	{
		return FVector::ZeroVector;
	}

	const FVector EnclosureLocation = Enclosure->GetActorLocation();

	switch (StaffCharacter->StaffType)
	{
	case EStaffType::Zookeeper:
	{
		// Find empty feeders near the enclosure.
		TArray<AActor*> AllFeeders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFeederActor::StaticClass(), AllFeeders);
		for (AActor* Actor : AllFeeders)
		{
			AFeederActor* Feeder = Cast<AFeederActor>(Actor);
			if (Feeder && Feeder->IsEmpty() && FVector::Dist(Feeder->GetActorLocation(), EnclosureLocation) < 3000.0f)
			{
				return Feeder->GetActorLocation();
			}
		}
		break;
	}

	case EStaffType::Veterinarian:
	{
		// Find sick animals in the enclosure.
		for (AAnimalBase* Animal : Enclosure->ContainedAnimals)
		{
			if (Animal && Animal->NeedsComponent)
			{
				if (Animal->NeedsComponent->GetNeedValue(FName("Health")) < 0.5f)
				{
					return Animal->GetActorLocation();
				}
			}
		}
		break;
	}

	case EStaffType::Mechanic:
	{
		// If enclosure itself is damaged, go to it.
		if (Enclosure->Condition < 0.5f)
		{
			return EnclosureLocation;
		}
		break;
	}

	case EStaffType::Janitor:
	case EStaffType::Guide:
	{
		// Janitors and guides patrol around the enclosure.
		return EnclosureLocation;
	}

	default:
		break;
	}

	// Fallback: go to enclosure center.
	return EnclosureLocation;
}

bool AStaffAIController::IsEnclosureNeedingAttention() const
{
	AStaffCharacter* StaffCharacter = Cast<AStaffCharacter>(GetPawn());
	if (!StaffCharacter)
	{
		return false;
	}

	AEnclosureActor* Enclosure = StaffCharacter->AssignedEnclosure;
	if (!Enclosure)
	{
		return false;
	}

	switch (StaffCharacter->StaffType)
	{
	case EStaffType::Zookeeper:
	{
		// Check for empty feeders near enclosure.
		TArray<AActor*> AllFeeders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFeederActor::StaticClass(), AllFeeders);
		const FVector EncLoc = Enclosure->GetActorLocation();
		for (AActor* Actor : AllFeeders)
		{
			AFeederActor* Feeder = Cast<AFeederActor>(Actor);
			if (Feeder && Feeder->IsEmpty() && FVector::Dist(Feeder->GetActorLocation(), EncLoc) < 3000.0f)
			{
				return true;
			}
		}
		return false;
	}

	case EStaffType::Veterinarian:
	{
		// Check for sick animals.
		for (AAnimalBase* Animal : Enclosure->ContainedAnimals)
		{
			if (Animal && Animal->NeedsComponent)
			{
				if (Animal->NeedsComponent->GetNeedValue(FName("Health")) < 0.5f)
				{
					return true;
				}
			}
		}
		return false;
	}

	case EStaffType::Mechanic:
	{
		return Enclosure->Condition < 0.5f;
	}

	case EStaffType::Janitor:
	case EStaffType::Guide:
	{
		// Always have work to do (patrolling/cleaning).
		return true;
	}

	default:
		return false;
	}
}
