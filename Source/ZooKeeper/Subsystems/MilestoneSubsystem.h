#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MilestoneSubsystem.generated.h"

/** Broadcast when a milestone is achieved for the first time. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMilestoneAchieved, FName, MilestoneID);

/**
 * UMilestoneSubsystem
 *
 * World subsystem that tracks milestone achievements.
 * Checked on day change and key events.
 *
 * Milestones:
 *   FirstSteps     - Place your first enclosure and acquire your first animal.
 *   GrowingZoo     - Have 5+ animals of 3+ species.
 *   Popular        - Reach 20+ visitors.
 *   Paradise       - Achieve a 4.0+ zoo rating.
 *   FiveStars      - Achieve a 5.0 zoo rating.
 *   Breeding       - Successfully breed an animal.
 *   Tycoon         - Accumulate $50,000+ funds.
 */
UCLASS(meta = (DisplayName = "Milestone Subsystem"))
class ZOOKEEPER_API UMilestoneSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	/** Checks all milestones and awards any that are newly met. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Milestones")
	void CheckMilestones();

	/** Returns true if the given milestone has been achieved. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Milestones")
	bool IsMilestoneAchieved(FName MilestoneID) const;

	/** Returns all achieved milestone IDs. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Milestones")
	TArray<FName> GetAchievedMilestones() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Milestones")
	FOnMilestoneAchieved OnMilestoneAchieved;

private:
	/** Called when the day changes to check milestones. */
	UFUNCTION()
	void HandleDayChanged(int32 NewDay);

	/** Awards a milestone if not already achieved. */
	void AwardMilestone(FName MilestoneID);

	/** Set of achieved milestone IDs. */
	TSet<FName> AchievedMilestones;
};
