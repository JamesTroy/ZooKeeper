#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherSubsystem.generated.h"

/**
 * EWeatherState
 *
 * Represents the current weather condition in the zoo.
 */
UENUM(BlueprintType)
enum class EWeatherState : uint8
{
	Clear    UMETA(DisplayName = "Clear"),
	Cloudy   UMETA(DisplayName = "Cloudy"),
	Rain     UMETA(DisplayName = "Rain"),
	Storm    UMETA(DisplayName = "Storm"),
	Snow     UMETA(DisplayName = "Snow"),
	Fog      UMETA(DisplayName = "Fog")
};

/** Broadcast when the weather state changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeatherState, NewWeather);

/**
 * UWeatherSubsystem
 *
 * World subsystem that simulates weather patterns for the zoo. Weather changes
 * periodically based on a configurable timer and is influenced by the current season.
 */
UCLASS(meta = (DisplayName = "Weather Subsystem"))
class ZOOKEEPER_API UWeatherSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Tick
	// -------------------------------------------------------------------

	/**
	 * Advances the weather simulation by the given game-time delta.
	 * Should be called each frame from the game mode.
	 * @param DeltaTime  Elapsed game-time seconds since the last tick.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Weather")
	void Tick(float DeltaTime);

	// -------------------------------------------------------------------
	//  Controls
	// -------------------------------------------------------------------

	/**
	 * Immediately sets the weather to the given state, resetting the change timer.
	 * @param NewWeather  The weather state to force.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Weather")
	void ForceWeather(EWeatherState NewWeather);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/**
	 * Returns the current ambient temperature in degrees Celsius,
	 * influenced by the season and weather state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Weather")
	float GetTemperature() const;

	/** Returns a localized display name for the current weather state. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Weather")
	FText GetWeatherDisplayName() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Weather")
	FOnWeatherChanged OnWeatherChanged;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** The current weather condition. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Weather")
	EWeatherState CurrentWeather;

	/** Countdown timer (in game seconds) until the next weather change. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Weather")
	float WeatherChangeTimer;

	/** Interval in game seconds between automatic weather changes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Weather", meta = (ClampMin = "1.0"))
	float WeatherChangePeriod;

private:
	/** Called when the season changes — triggers a weather change. */
	UFUNCTION()
	void HandleSeasonChanged(int32 NewSeason);

	/** Called each in-game hour to potentially change weather. */
	UFUNCTION()
	void HandleHourChanged(int32 NewHour);

	/**
	 * Selects a new random weather state weighted by the current season.
	 * @param Season  The current season index (0-3: Spring/Summer/Autumn/Winter).
	 * @return The new weather state.
	 */
	EWeatherState PickRandomWeather(int32 Season) const;

	/** Cached season for weather selection. */
	int32 CachedSeason;
};
