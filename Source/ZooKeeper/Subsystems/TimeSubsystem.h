#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeSubsystem.generated.h"

/** Broadcast when the time of day changes (fires every tick while unpaused). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, float, NewTime);

/** Broadcast when the day counter increments. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeDayChanged, int32, NewDay);

/** Broadcast when the season changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, int32, NewSeason);

/** Broadcast when a new in-game hour begins. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);

/**
 * UTimeSubsystem
 *
 * World subsystem responsible for tracking the zoo's in-game clock, day counter,
 * and season cycle. The game mode should call Tick() each frame to advance time.
 * 1 real second equals 1 game minute by default (GameTimeScale = 60).
 */
UCLASS(meta = (DisplayName = "Time Subsystem"))
class ZOOKEEPER_API UTimeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UTimeSubsystem();

	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Tick
	// -------------------------------------------------------------------

	/**
	 * Advances the in-game clock by the given real-world delta time.
	 * Should be called once per frame from the game mode.
	 * @param DeltaTime  Real-world seconds since the last frame.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void Tick(float DeltaTime);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns a human-readable time string, e.g. "14:35". */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Time")
	FText GetFormattedTime() const;

	/** Returns the current time of day as a 0-1 percentage (0 = midnight, 0.5 = noon). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Time")
	float GetTimeOfDayPercent() const;

	/**
	 * Returns a rotator representing the sun's orientation based on the current time.
	 * The sun rises at hour 6 and sets at hour 18, interpolating a 180-degree pitch arc.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Time")
	FRotator GetSunRotation() const;

	// -------------------------------------------------------------------
	//  Controls
	// -------------------------------------------------------------------

	/** Sets the game-time scale. A value of 60 means 1 real second = 1 game minute. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void SetTimeScale(float NewTimeScale);

	/** Pauses the flow of in-game time. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void PauseTime();

	/** Resumes the flow of in-game time. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void ResumeTime();

	/** Immediately advances to the start of the next day (time resets to 6:00). */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Time")
	void AdvanceToNextDay();

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Time")
	FOnTimeOfDayChanged OnTimeOfDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Time")
	FOnTimeDayChanged OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Time")
	FOnSeasonChanged OnSeasonChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Time")
	FOnHourChanged OnHourChanged;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** Multiplier converting real seconds to game-time seconds. Default 60 (1 real sec = 1 game minute). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Time", meta = (ClampMin = "0.0"))
	float GameTimeScale;

	/** Current time of day expressed as a float in the range [0, 24). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Time")
	float CurrentTimeOfDay;

	/** Current in-game day, starting at 1. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Time")
	int32 CurrentDay;

	/** Current season index: 0 = Spring, 1 = Summer, 2 = Autumn, 3 = Winter. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Time")
	int32 CurrentSeason;

	/** Whether the in-game clock is paused. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Time")
	bool bIsPaused;

	/** Hour at which the sun rises (default 6). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Time", meta = (ClampMin = "0.0", ClampMax = "12.0"))
	float SunriseHour;

	/** Hour at which the sun sets (default 18). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Time", meta = (ClampMin = "12.0", ClampMax = "24.0"))
	float SunsetHour;

private:
	/** The hour that was current on the previous tick, used to detect hour transitions. */
	int32 PreviousHour;
};
