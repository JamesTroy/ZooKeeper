#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "VisitorAIController.generated.h"

class UBehaviorTree;

/**
 * AVisitorAIController
 *
 * AI controller that drives visitor behavior using a behavior tree.
 * Provides utility functions for the behavior tree to find points
 * of interest within the zoo (attractions, food stalls, benches).
 */
UCLASS(Blueprintable, meta = (DisplayName = "Visitor AI Controller"))
class ZOOKEEPER_API AVisitorAIController : public AAIController
{
	GENERATED_BODY()

public:
	AVisitorAIController();

	//~ Begin AAIController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AAIController Interface

	// -------------------------------------------------------------------
	//  Utility Functions (for behavior tree tasks/services)
	// -------------------------------------------------------------------

	/**
	 * Finds the nearest attraction (e.g. enclosure with animals) to the controlled visitor.
	 * @return The nearest attraction actor, or nullptr if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitor|AI")
	AActor* FindNearestAttraction() const;

	/**
	 * Finds the nearest food stall to the controlled visitor.
	 * @return The nearest food stall actor, or nullptr if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitor|AI")
	AActor* FindFoodStall() const;

	/**
	 * Finds the nearest bench or resting spot to the controlled visitor.
	 * @return The nearest bench actor, or nullptr if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitor|AI")
	AActor* FindBench() const;

protected:
	/** The behavior tree asset that drives this visitor's decision-making. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Visitor|AI")
	TObjectPtr<UBehaviorTree> VisitorBehaviorTree;
};
