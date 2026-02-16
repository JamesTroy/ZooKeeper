#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalSleep.generated.h"

/**
 * UBTTask_AnimalSleep
 *
 * Simulates the animal sleeping. Replenishes energy through the needs
 * component and holds for the configured sleep duration.
 */
UCLASS(meta = (DisplayName = "Animal Sleep"))
class ZOOKEEPER_API UBTTask_AnimalSleep : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalSleep();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	/** Amount of energy restored (0-1 scale). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Sleep", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EnergyRestoreAmount;

	/** Duration in seconds the sleep action takes. */
	UPROPERTY(EditAnywhere, Category = "Zoo|Sleep", meta = (ClampMin = "0.1"))
	float SleepDuration;

private:
	float ElapsedTime;
};
