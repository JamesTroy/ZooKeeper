#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UBlackboardComponent;
class UAnimalNeedsComponent;

/**
 * AAnimalAIController
 *
 * AI controller that drives animal behavior through a behavior tree.
 * Periodically syncs the animal's needs component values into blackboard keys
 * so that BT decorators and tasks can make decisions.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Animal AI Controller"))
class ZOOKEEPER_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAnimalAIController();

	//~ Begin AAIController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AAIController Interface

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	// -------------------------------------------------------------------
	//  Behavior Tree Assets
	// -------------------------------------------------------------------

	/** The behavior tree to run for this animal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal AI")
	UBehaviorTree* AnimalBehaviorTree;

	/** The blackboard data asset defining the keys used by the behavior tree. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal AI")
	UBlackboardData* AnimalBlackboard;

private:
	/** Pushes current need values from the NeedsComponent into blackboard keys. */
	void UpdateBlackboardFromNeeds();

	/** Interval (seconds) between blackboard updates. */
	float BlackboardUpdateInterval;

	/** Accumulator for the update timer. */
	float BlackboardUpdateTimer;

	/** Cached pointer to the possessed animal's needs component. */
	UPROPERTY()
	UAnimalNeedsComponent* CachedNeedsComponent;
};
