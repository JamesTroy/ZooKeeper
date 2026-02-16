#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ZooGameState.generated.h"

/** Broadcast when the zoo's funds change. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFundsChanged, int32, NewFunds, int32, Delta);

/** Broadcast when the zoo's reputation changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReputationChanged, float, NewReputation, float, OldReputation);

/** Broadcast when a new day begins. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, NewDay);

/**
 * AZooGameState
 *
 * Replicated game state that tracks the zoo's economic status, reputation,
 * day/night cycle, weather, and visitor count. Provides authoritative
 * fund management with broadcast delegates for UI binding.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Game State"))
class ZOOKEEPER_API AZooGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AZooGameState();

	//~ Begin AActor Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

	// ---------------------------------------------------------------
	//  Economy
	// ---------------------------------------------------------------

	/**
	 * Adds the given amount to the current funds and broadcasts OnFundsChanged.
	 * @param Amount  The amount to add (must be positive).
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	void AddFunds(int32 Amount);

	/**
	 * Attempts to remove the given amount from the current funds.
	 * @param Amount  The amount to remove (must be positive).
	 * @return true if the funds were sufficient and the removal succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	bool RemoveFunds(int32 Amount);

	/** Returns the current funds. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Economy")
	int32 GetCurrentFunds() const { return CurrentFunds; }

	// ---------------------------------------------------------------
	//  Reputation
	// ---------------------------------------------------------------

	/**
	 * Sets the zoo reputation, clamped to [0, 5].
	 * Broadcasts OnReputationChanged when the value changes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Reputation")
	void SetReputation(float NewReputation);

	/** Returns the current reputation. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Reputation")
	float GetReputation() const { return ZooReputation; }

	// ---------------------------------------------------------------
	//  Day / Time
	// ---------------------------------------------------------------

	/**
	 * Advances to the next day. Broadcasts OnDayChanged.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void AdvanceDay();

	/**
	 * Returns a human-readable date string, e.g. "Day 7".
	 */
	UFUNCTION(BlueprintPure, Category = "Zoo|Time")
	FString GetFormattedDate() const;

	/** Returns the current day number. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Time")
	int32 GetCurrentDay() const { return CurrentDay; }

	// ---------------------------------------------------------------
	//  Weather
	// ---------------------------------------------------------------

	/** Sets the current weather state. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Weather")
	void SetWeather(FName NewWeather);

	/** Returns the current weather name. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Weather")
	FName GetWeather() const { return CurrentWeather; }

	// ---------------------------------------------------------------
	//  Visitors
	// ---------------------------------------------------------------

	/** Sets the current visitor count. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void SetVisitorCount(int32 Count);

	/** Returns the current number of visitors in the zoo. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Visitors")
	int32 GetVisitorCount() const { return VisitorCount; }

	// ---------------------------------------------------------------
	//  Delegates
	// ---------------------------------------------------------------

	/** Fired whenever CurrentFunds changes. */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|Economy")
	FOnFundsChanged OnFundsChanged;

	/** Fired whenever ZooReputation changes. */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|Reputation")
	FOnReputationChanged OnReputationChanged;

	/** Fired whenever CurrentDay changes. */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|Time")
	FOnDayChanged OnDayChanged;

protected:
	/** The zoo's current available funds. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zoo|Economy", meta = (AllowPrivateAccess = "true"))
	int32 CurrentFunds;

	/** The zoo's reputation score, ranging from 0 (terrible) to 5 (world-class). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zoo|Reputation", meta = (ClampMin = "0.0", ClampMax = "5.0", AllowPrivateAccess = "true"))
	float ZooReputation;

	/** The current in-game day number, starting from 1. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zoo|Time", meta = (AllowPrivateAccess = "true"))
	int32 CurrentDay;

	/** The current weather condition (e.g. "Sunny", "Rainy", "Snowy"). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zoo|Weather", meta = (AllowPrivateAccess = "true"))
	FName CurrentWeather;

	/** The number of visitors currently in the zoo. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zoo|Visitors", meta = (AllowPrivateAccess = "true"))
	int32 VisitorCount;
};
