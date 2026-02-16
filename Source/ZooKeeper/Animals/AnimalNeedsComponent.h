#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalNeedsComponent.generated.h"

/** Broadcast whenever a need value changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNeedChanged, FName, NeedName, float, NewValue);

/** Broadcast when any need drops below the critical threshold. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNeedCritical, FName, NeedName);

/**
 * UAnimalNeedsComponent
 *
 * Tracks the physiological and psychological needs of an animal.
 * Each need is a float in the range [0, 1] where 1 is fully satisfied.
 * Needs decay over time according to configurable rates.
 */
UCLASS(ClassGroup = (Zoo), meta = (BlueprintSpawnableComponent, DisplayName = "Animal Needs"))
class ZOOKEEPER_API UAnimalNeedsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimalNeedsComponent();

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	// -------------------------------------------------------------------
	//  Need Values (0.0 = empty, 1.0 = full)
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Hunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Thirst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Happiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Social;

	// -------------------------------------------------------------------
	//  Decay Rates (units per second)
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float HungerDecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float ThirstDecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float EnergyDecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float HealthDecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float HappinessDecayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal Needs|Decay", meta = (ClampMin = "0.0"))
	float SocialDecayRate;

	// -------------------------------------------------------------------
	//  Actions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|Animal Needs")
	void FeedAnimal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Animal Needs")
	void GiveWater(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Animal Needs")
	void ReplenishEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Animal Needs")
	void Socialize(float Amount);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns the name of the need with the lowest value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animal Needs")
	FName GetMostUrgentNeed() const;

	/** Returns the average of all need values as an overall wellbeing score. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animal Needs")
	float GetOverallWellbeing() const;

	/** Returns true if any need is below the critical threshold (0.15). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animal Needs")
	bool IsAnyCritical() const;

	/** Returns the value of a specific need by name. Returns -1.0 if the name is invalid. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animal Needs")
	float GetNeedValue(FName NeedName) const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animal Needs")
	FOnNeedChanged OnNeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animal Needs")
	FOnNeedCritical OnNeedCritical;

private:
	/** Critical threshold below which a need fires the OnNeedCritical delegate. */
	static constexpr float CriticalThreshold = 0.15f;

	/** Helper: apply decay to a single need, broadcast changes, and check critical state. */
	void DecayNeed(float& NeedValue, float DecayRate, float DeltaTime, FName NeedName);

	/** Helper: set a need value, clamping and broadcasting. */
	void SetNeedValue(float& NeedValue, float NewValue, FName NeedName);
};
