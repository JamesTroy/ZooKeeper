#include "RandomEventSubsystem.h"
#include "TimeSubsystem.h"
#include "ZooKeeper.h"

bool URandomEventSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URandomEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EventChance = 0.15f; // 15% chance per hour
	LastEventID = NAME_None;

	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->OnHourChanged.AddDynamic(this, &URandomEventSubsystem::HandleHourChanged);
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("RandomEventSubsystem::Initialize - Event chance: %.0f%%"), EventChance * 100.0f);
}

void URandomEventSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("RandomEventSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void URandomEventSubsystem::HandleHourChanged(int32 NewHour)
{
	// Only roll events during zoo operating hours (8-20)
	if (NewHour >= 8 && NewHour <= 20)
	{
		RollRandomEvent();
	}
}

void URandomEventSubsystem::RollRandomEvent()
{
	if (FMath::FRand() > EventChance)
	{
		return; // No event this hour
	}

	// Weighted random event selection
	struct FEventWeight
	{
		FName EventID;
		float Weight;
	};

	static const TArray<FEventWeight> Events = {
		{ FName("AnimalSick"),     25.0f },
		{ FName("AnimalEscape"),   10.0f },
		{ FName("VIPVisitor"),     20.0f },
		{ FName("Inspection"),     15.0f },
		{ FName("StormDamage"),    15.0f },
		{ FName("Protest"),         5.0f },
		{ FName("DonationReceived"),10.0f }
	};

	float TotalWeight = 0.0f;
	for (const FEventWeight& E : Events)
	{
		TotalWeight += E.Weight;
	}

	float Roll = FMath::FRandRange(0.0f, TotalWeight);
	float Accumulator = 0.0f;

	FName SelectedEvent = Events[0].EventID;
	for (const FEventWeight& E : Events)
	{
		Accumulator += E.Weight;
		if (Roll < Accumulator)
		{
			SelectedEvent = E.EventID;
			break;
		}
	}

	LastEventID = SelectedEvent;
	OnRandomEvent.Broadcast(SelectedEvent);

	UE_LOG(LogZooKeeper, Log, TEXT("RandomEventSubsystem - Event fired: '%s'"), *SelectedEvent.ToString());
}
