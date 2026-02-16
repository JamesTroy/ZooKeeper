#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalEat.generated.h"

/**
 * UBTTask_AnimalEat
 *
 * Simulates the animal eating. Feeds the animal directly through its
 * needs component and waits for the eat animation duration.
 */
UCLASS(meta = (DisplayName = "Animal Eat"))
class ZOOKEEPER_API UBTTask_AnimalEat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalEat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	/** Amount of hunger restored (0-1 scale). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Eat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FeedAmount;

	/** Duration in seconds the eat action takes. */
	UPROPERTY(EditAnywhere, Category = "Zoo|Eat", meta = (ClampMin = "0.1"))
	float EatDuration;

private:
	/** Time elapsed since the eat action started. */
	float ElapsedTime;
};
