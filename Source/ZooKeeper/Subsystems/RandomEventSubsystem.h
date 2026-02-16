#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RandomEventSubsystem.generated.h"

/** Broadcast when a random event occurs. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRandomEvent, FName, EventID);

/**
 * URandomEventSubsystem
 *
 * World subsystem that rolls random events periodically during gameplay.
 * Events include animal escapes, sickness, VIP visitors, inspections,
 * storm damage, and protests.
 */
UCLASS(meta = (DisplayName = "Random Event Subsystem"))
class ZOOKEEPER_API URandomEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	/** Checks for and potentially fires a random event. Called on hour change. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Events")
	void RollRandomEvent();

	/** Returns the last event that occurred. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Events")
	FName GetLastEvent() const { return LastEventID; }

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Events")
	FOnRandomEvent OnRandomEvent;

	/** Chance (0-1) of an event occurring each roll. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Events", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EventChance;

private:
	/** Called when the hour changes to check for events. */
	UFUNCTION()
	void HandleHourChanged(int32 NewHour);

	FName LastEventID;
};
