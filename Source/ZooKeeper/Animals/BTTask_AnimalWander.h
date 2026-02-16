#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalWander.generated.h"

/**
 * UBTTask_AnimalWander
 *
 * Picks a random navigable point within the animal's enclosure bounds
 * and moves the AI pawn toward it.
 */
UCLASS(meta = (DisplayName = "Animal Wander"))
class ZOOKEEPER_API UBTTask_AnimalWander : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalWander();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	/** Maximum radius (in cm) from the animal's current location to wander. */
	UPROPERTY(EditAnywhere, Category = "Zoo|Wander", meta = (ClampMin = "100.0"))
	float WanderRadius;

private:
	/** Whether the AI is currently moving toward the wander target. */
	bool bIsMoving;
};
