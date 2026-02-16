#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ZooRatingSubsystem.generated.h"

/** Broadcast when the zoo's star rating changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRatingChanged, float, NewRating);

/**
 * UZooRatingSubsystem
 *
 * World subsystem that calculates the zoo's overall star rating (0-5).
 * Rating is computed from weighted factors:
 *   - AnimalDiversity (0.25)
 *   - AnimalHappiness (0.25)
 *   - VisitorSatisfaction (0.20)
 *   - EnclosureQuality (0.15)
 *   - PathAndAmenities (0.15)
 * Drives visitor spawn rate: VisitorsPerHour = BaseRate * (1 + Rating * 0.5).
 */
UCLASS(meta = (DisplayName = "Zoo Rating Subsystem"))
class ZOOKEEPER_API UZooRatingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	/** Recalculates the zoo rating from all contributing factors. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Rating")
	void RecalculateRating();

	/** Returns the current zoo rating (0-5 stars). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Rating")
	float GetRating() const { return CurrentRating; }

	/** Returns the visitor spawn rate multiplier based on current rating. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Rating")
	float GetVisitorSpawnMultiplier() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Rating")
	FOnRatingChanged OnRatingChanged;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float CurrentRating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float AnimalDiversityScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float AnimalHappinessScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float VisitorSatisfactionScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float EnclosureQualityScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Rating")
	float AmenityScore;

private:
	/** Called when the day changes to recalculate the rating. */
	UFUNCTION()
	void HandleDayChanged(int32 NewDay);
};
