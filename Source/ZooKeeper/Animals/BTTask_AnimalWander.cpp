#include "BTTask_AnimalWander.h"
#include "AnimalBase.h"
#include "Buildings/EnclosureActor.h"
#include "ZooKeeper.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AnimalWander::UBTTask_AnimalWander()
{
	NodeName  = TEXT("Animal Wander");
	bNotifyTick = true;
	bIsMoving = false;
	WanderRadius = 500.0f;
}

EBTNodeResult::Type UBTTask_AnimalWander::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                       uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BTTask_AnimalWander: No navigation system found."));
		return EBTNodeResult::Failed;
	}

	// Determine wander target. Prefer using the enclosure volume for bounded wandering.
	FVector WanderTarget;

	AAnimalBase* Animal = Cast<AAnimalBase>(Pawn);
	AEnclosureActor* Enclosure = Animal ? Cast<AEnclosureActor>(Animal->CurrentEnclosure) : nullptr;

	if (Enclosure)
	{
		// Use the enclosure's volume to get a random point within its actual polygon bounds.
		WanderTarget = Enclosure->GetRandomPointInEnclosure();
	}
	else
	{
		// Fallback: random reachable point near the animal's current position.
		FNavLocation ResultLocation;
		const bool bFound = NavSys->GetRandomReachablePointInRadius(
			Pawn->GetActorLocation(), WanderRadius, ResultLocation);

		if (!bFound)
		{
			UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalWander: no navigable point found."));
			return EBTNodeResult::Failed;
		}

		WanderTarget = ResultLocation.Location;
	}

	AIController->MoveToLocation(WanderTarget, 50.0f);
	bIsMoving = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalWander::TickTask(UBehaviorTreeComponent& OwnerComp,
                                     uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check if the AI has finished moving.
	const EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
	if (MoveStatus != EPathFollowingStatus::Moving)
	{
		bIsMoving = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_AnimalWander::GetStaticDescription() const
{
	return FString::Printf(TEXT("Wander within %.0f cm radius"), WanderRadius);
}
