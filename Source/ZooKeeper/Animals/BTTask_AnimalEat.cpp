#include "BTTask_AnimalEat.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"
#include "AIController.h"

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

	// Feed the animal immediately; the task duration simulates the eating animation.
	Animal->NeedsComponent->FeedAnimal(FeedAmount);
	ElapsedTime = 0.0f;

	UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalEat: '%s' started eating."), *Animal->AnimalName);

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
