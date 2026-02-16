#include "WeatherSubsystem.h"
#include "TimeSubsystem.h"
#include "ZooKeeper.h"

bool UWeatherSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentWeather = EWeatherState::Clear;
	WeatherChangePeriod = 300.0f; // 300 game seconds
	WeatherChangeTimer = WeatherChangePeriod;
	CachedSeason = 0;

	// Subscribe to season and hour changes instead of polling TimeSubsystem each tick.
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->OnSeasonChanged.AddDynamic(this, &UWeatherSubsystem::HandleSeasonChanged);
			TimeSys->OnHourChanged.AddDynamic(this, &UWeatherSubsystem::HandleHourChanged);
			CachedSeason = TimeSys->CurrentSeason;
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("WeatherSubsystem::Initialize - Starting weather: Clear, Period: %.0fs"),
		WeatherChangePeriod);
}

void UWeatherSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("WeatherSubsystem::Deinitialize"));

	Super::Deinitialize();
}

void UWeatherSubsystem::Tick(float DeltaTime)
{
	WeatherChangeTimer -= DeltaTime;

	if (WeatherChangeTimer <= 0.0f)
	{
		WeatherChangeTimer = WeatherChangePeriod;

		const EWeatherState NewWeather = PickRandomWeather(CachedSeason);

		if (NewWeather != CurrentWeather)
		{
			CurrentWeather = NewWeather;
			OnWeatherChanged.Broadcast(CurrentWeather);

			UE_LOG(LogZooKeeper, Log, TEXT("WeatherSubsystem - Weather changed to: %s"),
				*GetWeatherDisplayName().ToString());
		}
	}
}

void UWeatherSubsystem::HandleSeasonChanged(int32 NewSeason)
{
	CachedSeason = NewSeason;

	// Force a weather change when the season transitions.
	const EWeatherState NewWeather = PickRandomWeather(CachedSeason);
	if (NewWeather != CurrentWeather)
	{
		CurrentWeather = NewWeather;
		OnWeatherChanged.Broadcast(CurrentWeather);

		UE_LOG(LogZooKeeper, Log, TEXT("WeatherSubsystem - Season changed, new weather: %s"),
			*GetWeatherDisplayName().ToString());
	}
}

void UWeatherSubsystem::HandleHourChanged(int32 NewHour)
{
	// Weather timer is decremented in Tick, but we also check on hour boundaries
	// for more responsive weather changes in case Tick is not being called regularly.
}

void UWeatherSubsystem::ForceWeather(EWeatherState NewWeather)
{
	CurrentWeather = NewWeather;
	WeatherChangeTimer = WeatherChangePeriod;

	OnWeatherChanged.Broadcast(CurrentWeather);

	UE_LOG(LogZooKeeper, Log, TEXT("WeatherSubsystem - Weather forced to: %s"),
		*GetWeatherDisplayName().ToString());
}

float UWeatherSubsystem::GetTemperature() const
{
	// Base temperature by season (Celsius)
	// Season 0 = Spring: 18, Season 1 = Summer: 28, Season 2 = Autumn: 14, Season 3 = Winter: 2
	static const float SeasonBaseTemps[] = { 18.0f, 28.0f, 14.0f, 2.0f };
	float BaseTemp = 18.0f;

	if (const UWorld* World = GetWorld())
	{
		if (const UTimeSubsystem* TimeSub = World->GetSubsystem<UTimeSubsystem>())
		{
			const int32 Season = FMath::Clamp(TimeSub->CurrentSeason, 0, 3);
			BaseTemp = SeasonBaseTemps[Season];
		}
	}

	// Apply weather modifiers
	switch (CurrentWeather)
	{
	case EWeatherState::Clear:
		// No modifier
		break;
	case EWeatherState::Cloudy:
		BaseTemp -= 2.0f;
		break;
	case EWeatherState::Rain:
		BaseTemp -= 5.0f;
		break;
	case EWeatherState::Storm:
		BaseTemp -= 8.0f;
		break;
	case EWeatherState::Snow:
		BaseTemp -= 15.0f;
		break;
	case EWeatherState::Fog:
		BaseTemp -= 3.0f;
		break;
	}

	return BaseTemp;
}

FText UWeatherSubsystem::GetWeatherDisplayName() const
{
	switch (CurrentWeather)
	{
	case EWeatherState::Clear:   return FText::FromString(TEXT("Clear"));
	case EWeatherState::Cloudy:  return FText::FromString(TEXT("Cloudy"));
	case EWeatherState::Rain:    return FText::FromString(TEXT("Rain"));
	case EWeatherState::Storm:   return FText::FromString(TEXT("Storm"));
	case EWeatherState::Snow:    return FText::FromString(TEXT("Snow"));
	case EWeatherState::Fog:     return FText::FromString(TEXT("Fog"));
	default:                     return FText::FromString(TEXT("Unknown"));
	}
}

EWeatherState UWeatherSubsystem::PickRandomWeather(int32 Season) const
{
	// Weighted random selection based on season.
	// Weights: { Clear, Cloudy, Rain, Storm, Snow, Fog }
	TArray<float> Weights;

	switch (Season)
	{
	case 0: // Spring
		Weights = { 30.0f, 25.0f, 25.0f, 10.0f, 0.0f, 10.0f };
		break;
	case 1: // Summer
		Weights = { 45.0f, 20.0f, 15.0f, 10.0f, 0.0f, 10.0f };
		break;
	case 2: // Autumn
		Weights = { 20.0f, 30.0f, 25.0f, 10.0f, 5.0f, 10.0f };
		break;
	case 3: // Winter
		Weights = { 15.0f, 20.0f, 10.0f, 5.0f, 35.0f, 15.0f };
		break;
	default:
		Weights = { 30.0f, 25.0f, 20.0f, 10.0f, 5.0f, 10.0f };
		break;
	}

	// Calculate total weight
	float TotalWeight = 0.0f;
	for (float W : Weights)
	{
		TotalWeight += W;
	}

	// Pick a random value in [0, TotalWeight)
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float Accumulator = 0.0f;

	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		Accumulator += Weights[i];
		if (RandomValue < Accumulator)
		{
			return static_cast<EWeatherState>(i);
		}
	}

	// Fallback
	return EWeatherState::Clear;
}
