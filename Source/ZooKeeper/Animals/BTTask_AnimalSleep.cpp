#include "BTTask_AnimalSleep.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"
#include "AIController.h"

UBTTask_AnimalSleep::UBTTask_AnimalSleep()
{
	NodeName           = TEXT("Animal Sleep");
	bNotifyTick        = true;
	EnergyRestoreAmount = 0.5f;
	SleepDuration      = 5.0f;
	ElapsedTime        = 0.0f;
}

EBTNodeResult::Type UBTTask_AnimalSleep::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
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

	// Replenish energy immediately; the task duration simulates the sleep animation.
	Animal->NeedsComponent->ReplenishEnergy(EnergyRestoreAmount);
	ElapsedTime = 0.0f;

	UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalSleep: '%s' started sleeping."), *Animal->AnimalName);

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalSleep::TickTask(UBehaviorTreeComponent& OwnerComp,
                                    uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime >= SleepDuration)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("BTTask_AnimalSleep: finished sleeping."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_AnimalSleep::GetStaticDescription() const
{
	return FString::Printf(TEXT("Sleep for %.1fs (restore %.0f%% energy)"),
	                        SleepDuration, EnergyRestoreAmount * 100.0f);
}
