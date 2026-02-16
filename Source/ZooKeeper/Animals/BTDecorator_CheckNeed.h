#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckNeed.generated.h"

/**
 * UBTDecorator_CheckNeed
 *
 * BT decorator that reads a named need value from the blackboard and
 * compares it against a configurable threshold. Can be set to pass when
 * the need is below OR above the threshold.
 */
UCLASS(meta = (DisplayName = "Check Animal Need"))
class ZOOKEEPER_API UBTDecorator_CheckNeed : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckNeed();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	/** Name of the blackboard key that stores the need value (e.g. "Hunger"). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Need Check")
	FName NeedName;

	/** Threshold to compare against (0-1). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Need Check", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Threshold;

	/**
	 * If true, the condition passes when the need is BELOW the threshold
	 * (i.e. the animal is in need). If false, passes when ABOVE.
	 */
	UPROPERTY(EditAnywhere, Category = "Zoo|Need Check")
	bool bCheckBelow;
};
