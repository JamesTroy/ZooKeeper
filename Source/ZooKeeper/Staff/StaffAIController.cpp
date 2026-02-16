#include "StaffAIController.h"
#include "StaffCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffAIController [%s] FindTaskInEnclosure: no valid StaffCharacter pawn."),
			*GetName());
		return FVector::ZeroVector;
	}

	AEnclosureActor* Enclosure = StaffCharacter->AssignedEnclosure;
	if (!Enclosure)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("Staff [%s] has no assigned enclosure; no task location available."),
			*StaffCharacter->StaffName);
		return FVector::ZeroVector;
	}

	// Return the enclosure's location as the task destination.
	// In a full implementation, this would query the enclosure for specific
	// task points (dirty areas, hungry animals, broken structures, etc.)
	// based on the staff member's type.
	return Enclosure->GetActorLocation();
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

	// In a full implementation, this would check enclosure state:
	// - Zookeeper: are any animals hungry/thirsty?
	// - Veterinarian: are any animals sick or injured?
	// - Mechanic: are any structures damaged?
	// - Janitor: is the area dirty?
	// - Guide: are visitors nearby who need assistance?
	//
	// For now, always return true if an enclosure is assigned,
	// indicating there is always potential work to be done.
	return true;
}
