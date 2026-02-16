#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalSocialize.generated.h"

/**
 * UBTTask_AnimalSocialize
 *
 * Finds the nearest animal of the same species within the enclosure,
 * moves toward it, and applies a social need boost to both animals.
 */
UCLASS(meta = (DisplayName = "Animal Socialize"))
class ZOOKEEPER_API UBTTask_AnimalSocialize : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalSocialize();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	/** Social need amount applied to both animals. */
	UPROPERTY(EditAnywhere, Category = "Zoo|Socialize", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SocializeAmount;

	/** Maximum distance to search for a partner (cm). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Socialize", meta = (ClampMin = "100.0"))
	float SearchRadius;

	/** Distance at which the animals are considered close enough to socialize (cm). */
	UPROPERTY(EditAnywhere, Category = "Zoo|Socialize", meta = (ClampMin = "10.0"))
	float InteractionDistance;

private:
	enum class ESocializePhase : uint8
	{
		MovingToPartner,
		Socializing,
	};

	ESocializePhase CurrentPhase;
	float ElapsedTime;
	float SocializeDuration;

	UPROPERTY()
	TWeakObjectPtr<AActor> PartnerActor;
};
