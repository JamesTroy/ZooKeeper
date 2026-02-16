#include "BTDecorator_CheckNeed.h"
#include "ZooKeeper.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckNeed::UBTDecorator_CheckNeed()
{
	NodeName   = TEXT("Check Animal Need");
	NeedName   = FName("Hunger");
	Threshold  = 0.3f;
	bCheckBelow = true;
}

bool UBTDecorator_CheckNeed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                         uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BTDecorator_CheckNeed: no blackboard component."));
		return false;
	}

	const float NeedValue = BB->GetValueAsFloat(NeedName);

	if (bCheckBelow)
	{
		return NeedValue < Threshold;
	}
	else
	{
		return NeedValue >= Threshold;
	}
}

FString UBTDecorator_CheckNeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Need '%s' %s %.2f"),
	                        *NeedName.ToString(),
	                        bCheckBelow ? TEXT("<") : TEXT(">="),
	                        Threshold);
}
