#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateNeeds.generated.h"

/**
 * UBTService_UpdateNeeds
 *
 * BT service that periodically reads values from the animal's needs component
 * and writes them into the behavior tree's blackboard so that decorators and
 * tasks can make data-driven decisions.
 */
UCLASS(meta = (DisplayName = "Update Animal Needs"))
class ZOOKEEPER_API UBTService_UpdateNeeds : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateNeeds();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
};
