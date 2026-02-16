#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StaffAIController.generated.h"

class UBehaviorTree;

/**
 * AStaffAIController
 *
 * AI controller that drives staff member behavior using a behavior tree.
 * Provides utility functions for the behavior tree to locate tasks
 * within the staff member's assigned enclosure.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Staff AI Controller"))
class ZOOKEEPER_API AStaffAIController : public AAIController
{
	GENERATED_BODY()

public:
	AStaffAIController();

	//~ Begin AAIController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AAIController Interface

	// -------------------------------------------------------------------
	//  Utility Functions (for behavior tree tasks/services)
	// -------------------------------------------------------------------

	/**
	 * Finds a task location within the staff member's assigned enclosure.
	 * Returns a world-space position where work needs to be done (e.g. a dirty
	 * area, a hungry animal, a broken fence).
	 * @return A world-space location for the next task, or FVector::ZeroVector if none found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff|AI")
	FVector FindTaskInEnclosure() const;

	/**
	 * Checks whether the staff member's assigned enclosure currently needs attention.
	 * @return true if there is pending work in the enclosure.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff|AI")
	bool IsEnclosureNeedingAttention() const;

protected:
	/** The behavior tree asset that drives this staff member's AI logic. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Staff|AI")
	TObjectPtr<UBehaviorTree> StaffBehaviorTree;
};
