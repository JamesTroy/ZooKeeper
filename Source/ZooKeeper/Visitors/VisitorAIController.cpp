#include "VisitorAIController.h"
#include "VisitorCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZooKeeper.h"

AVisitorAIController::AVisitorAIController()
{
	VisitorBehaviorTree = nullptr;
}

void AVisitorAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (VisitorBehaviorTree)
	{
		RunBehaviorTree(VisitorBehaviorTree);
		UE_LOG(LogZooKeeper, Log, TEXT("VisitorAIController [%s] started behavior tree for pawn [%s]."),
			*GetName(), *InPawn->GetName());
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("VisitorAIController [%s] has no VisitorBehaviorTree assigned."),
			*GetName());
	}
}

void AVisitorAIController::OnUnPossess()
{
	UBrainComponent* BrainComp = GetBrainComponent();
	if (BrainComp)
	{
		BrainComp->StopLogic(TEXT("Unpossessed"));
	}

	Super::OnUnPossess();
}

void AVisitorAIController::InvalidateCache()
{
	CachedAttractions.Empty();
	CachedFoodStalls.Empty();
	CachedBenches.Empty();
}

void AVisitorAIController::EnsureCacheValid(TArray<AActor*>& Cache, const FName& Tag) const
{
	if (Cache.Num() == 0)
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, Cache);
	}
}

AActor* AVisitorAIController::FindNearestFromCache(TArray<AActor*>& Cache, const FName& Tag) const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return nullptr;
	}

	EnsureCacheValid(Cache, Tag);

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	AActor* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : Cache)
	{
		if (!Actor)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PawnLocation, Actor->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Actor;
		}
	}

	return Nearest;
}

AActor* AVisitorAIController::FindNearestAttraction() const
{
	return FindNearestFromCache(CachedAttractions, FName(TEXT("Attraction")));
}

AActor* AVisitorAIController::FindFoodStall() const
{
	return FindNearestFromCache(CachedFoodStalls, FName(TEXT("FoodStall")));
}

AActor* AVisitorAIController::FindBench() const
{
	return FindNearestFromCache(CachedBenches, FName(TEXT("Bench")));
}
