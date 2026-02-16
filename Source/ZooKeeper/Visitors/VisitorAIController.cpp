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

AActor* AVisitorAIController::FindNearestAttraction() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return nullptr;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();

	// Find all actors tagged as attractions (enclosures with animals)
	TArray<AActor*> Attractions;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Attraction")), Attractions);

	AActor* NearestAttraction = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (AActor* Attraction : Attractions)
	{
		if (!Attraction)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PawnLocation, Attraction->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestAttraction = Attraction;
		}
	}

	return NearestAttraction;
}

AActor* AVisitorAIController::FindFoodStall() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return nullptr;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();

	// Find all actors tagged as food stalls
	TArray<AActor*> FoodStalls;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("FoodStall")), FoodStalls);

	AActor* NearestStall = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (AActor* Stall : FoodStalls)
	{
		if (!Stall)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PawnLocation, Stall->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestStall = Stall;
		}
	}

	return NearestStall;
}

AActor* AVisitorAIController::FindBench() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return nullptr;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();

	// Find all actors tagged as benches / resting spots
	TArray<AActor*> Benches;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Bench")), Benches);

	AActor* NearestBench = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (AActor* Bench : Benches)
	{
		if (!Bench)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PawnLocation, Bench->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestBench = Bench;
		}
	}

	return NearestBench;
}
