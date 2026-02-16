#include "TimeSubsystem.h"
#include "ZooKeeper.h"

UTimeSubsystem::UTimeSubsystem()
	: GameTimeScale(60.0f)
	, CurrentTimeOfDay(6.0f) // Start at 6 AM
	, CurrentDay(1)
	, CurrentSeason(0) // Spring
	, bIsPaused(false)
	, SunriseHour(6.0f)
	, SunsetHour(18.0f)
	, PreviousHour(6)
{
}

bool UTimeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem::Initialize - Day %d, Season %d, Time %.2f"),
		CurrentDay, CurrentSeason, CurrentTimeOfDay);
}

void UTimeSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem::Deinitialize - Final state: Day %d, Season %d, Time %.2f"),
		CurrentDay, CurrentSeason, CurrentTimeOfDay);

	Super::Deinitialize();
}

void UTimeSubsystem::Tick(float DeltaTime)
{
	if (bIsPaused)
	{
		return;
	}

	// Convert real-world delta to game-time hours.
	// GameTimeScale is in game-seconds per real-second.
	// DeltaTime is in real seconds.
	// Game seconds elapsed = DeltaTime * GameTimeScale
	// Game hours elapsed = game seconds / 3600
	const float GameSecondsElapsed = DeltaTime * GameTimeScale;
	const float GameHoursElapsed = GameSecondsElapsed / 3600.0f;

	CurrentTimeOfDay += GameHoursElapsed;

	// Check for day rollover
	while (CurrentTimeOfDay >= 24.0f)
	{
		CurrentTimeOfDay -= 24.0f;
		CurrentDay++;

		OnDayChanged.Broadcast(CurrentDay);

		UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - New day: %d"), CurrentDay);

		// Season changes every 7 days (days 1-7 = Spring, 8-14 = Summer, etc.)
		const int32 NewSeason = ((CurrentDay - 1) / 7) % 4;
		if (NewSeason != CurrentSeason)
		{
			CurrentSeason = NewSeason;
			OnSeasonChanged.Broadcast(CurrentSeason);

			static const TCHAR* SeasonNames[] = { TEXT("Spring"), TEXT("Summer"), TEXT("Autumn"), TEXT("Winter") };
			UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - Season changed to: %s"), SeasonNames[CurrentSeason]);
		}
	}

	// Detect hour transitions
	const int32 CurrentHour = FMath::FloorToInt(CurrentTimeOfDay);
	if (CurrentHour != PreviousHour)
	{
		PreviousHour = CurrentHour;
		OnHourChanged.Broadcast(CurrentHour);
	}

	OnTimeOfDayChanged.Broadcast(CurrentTimeOfDay);
}

FText UTimeSubsystem::GetFormattedTime() const
{
	const int32 Hours = FMath::FloorToInt(CurrentTimeOfDay);
	const int32 Minutes = FMath::FloorToInt(FMath::Frac(CurrentTimeOfDay) * 60.0f);

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Hours, Minutes));
}

float UTimeSubsystem::GetTimeOfDayPercent() const
{
	return CurrentTimeOfDay / 24.0f;
}

void UTimeSubsystem::SetTimeScale(float NewTimeScale)
{
	GameTimeScale = FMath::Max(0.0f, NewTimeScale);

	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - Time scale set to: %.2f"), GameTimeScale);
}

void UTimeSubsystem::PauseTime()
{
	bIsPaused = true;

	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - Time paused."));
}

void UTimeSubsystem::ResumeTime()
{
	bIsPaused = false;

	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - Time resumed."));
}

void UTimeSubsystem::AdvanceToNextDay()
{
	CurrentDay++;
	CurrentTimeOfDay = 6.0f; // Start the new day at 6 AM
	PreviousHour = 6;

	OnDayChanged.Broadcast(CurrentDay);

	UE_LOG(LogZooKeeper, Log, TEXT("TimeSubsystem - Advanced to day %d"), CurrentDay);

	// Check for season change
	const int32 NewSeason = ((CurrentDay - 1) / 7) % 4;
	if (NewSeason != CurrentSeason)
	{
		CurrentSeason = NewSeason;
		OnSeasonChanged.Broadcast(CurrentSeason);
	}
}

FRotator UTimeSubsystem::GetSunRotation() const
{
	// Sun rises at SunriseHour, sets at SunsetHour (configurable).
	const float DaylightDuration = SunsetHour - SunriseHour;
	const float NightDuration = 24.0f - DaylightDuration;

	float SunPitch;

	if (CurrentTimeOfDay >= SunriseHour && CurrentTimeOfDay <= SunsetHour)
	{
		// Daytime: map [Sunrise, Sunset] -> [0, 1] -> sin arc
		const float DayProgress = (CurrentTimeOfDay - SunriseHour) / DaylightDuration;
		SunPitch = FMath::Sin(DayProgress * PI) * 90.0f;
	}
	else
	{
		// Nighttime: sun is below horizon
		float NightProgress;
		if (CurrentTimeOfDay > SunsetHour)
		{
			NightProgress = (CurrentTimeOfDay - SunsetHour) / NightDuration;
		}
		else
		{
			NightProgress = (CurrentTimeOfDay + (24.0f - SunsetHour)) / NightDuration;
		}
		SunPitch = -FMath::Sin(NightProgress * PI) * 90.0f;
	}

	// Yaw rotates the sun east-to-west over the course of the day
	const float SunYaw = (CurrentTimeOfDay / 24.0f) * 360.0f - 90.0f;

	return FRotator(SunPitch, SunYaw, 0.0f);
}
