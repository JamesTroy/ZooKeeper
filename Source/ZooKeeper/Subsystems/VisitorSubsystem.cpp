#include "VisitorSubsystem.h"
#include "ZooKeeper.h"

bool UVisitorSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UVisitorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MaxVisitors = 50;
	CurrentVisitorCount = 0;
	AverageSatisfaction = 50.0f;

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem::Initialize - MaxVisitors: %d"), MaxVisitors);
}

void UVisitorSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem::Deinitialize - %d visitors at shutdown."), CurrentVisitorCount);

	AllVisitorCharacters.Empty();

	Super::Deinitialize();
}

void UVisitorSubsystem::RegisterVisitor(AVisitorCharacter* Visitor)
{
	if (!Visitor)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("VisitorSubsystem::RegisterVisitor - Null visitor passed."));
		return;
	}

	if (AllVisitorCharacters.Contains(Visitor))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("VisitorSubsystem::RegisterVisitor - Visitor already registered."));
		return;
	}

	AllVisitorCharacters.Add(Visitor);
	CurrentVisitorCount = AllVisitorCharacters.Num();
	OnVisitorCountChanged.Broadcast(CurrentVisitorCount);

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Visitor registered. Total: %d"), CurrentVisitorCount);
}

void UVisitorSubsystem::UnregisterVisitor(AVisitorCharacter* Visitor)
{
	if (!Visitor)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("VisitorSubsystem::UnregisterVisitor - Null visitor passed."));
		return;
	}

	const int32 Removed = AllVisitorCharacters.Remove(Visitor);
	if (Removed > 0)
	{
		CurrentVisitorCount = AllVisitorCharacters.Num();
		OnVisitorCountChanged.Broadcast(CurrentVisitorCount);

		UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Visitor unregistered. Total: %d"), CurrentVisitorCount);
	}
}

void UVisitorSubsystem::SpawnVisitors(int32 Count)
{
	if (Count <= 0)
	{
		return;
	}

	const int32 AvailableSlots = MaxVisitors - CurrentVisitorCount;
	const int32 ActualSpawn = FMath::Min(Count, AvailableSlots);

	if (ActualSpawn <= 0)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem::SpawnVisitors - Zoo is at capacity (%d/%d)."),
			CurrentVisitorCount, MaxVisitors);
		return;
	}

	const int32 OldCount = CurrentVisitorCount;
	CurrentVisitorCount += ActualSpawn;

	OnVisitorCountChanged.Broadcast(CurrentVisitorCount);

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Spawned %d visitors (%d -> %d)."),
		ActualSpawn, OldCount, CurrentVisitorCount);
}

void UVisitorSubsystem::DespawnAllVisitors()
{
	if (CurrentVisitorCount == 0)
	{
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Despawning all %d visitors."), CurrentVisitorCount);

	CurrentVisitorCount = 0;
	OnVisitorCountChanged.Broadcast(CurrentVisitorCount);
}

int32 UVisitorSubsystem::CalculateVisitorAttraction() const
{
	// Base attraction is influenced by:
	// - Number of different animal species
	// - Average visitor satisfaction
	// - Zoo reputation (would query game state)
	// For now, use a simple formula based on satisfaction.

	const float SatisfactionFactor = AverageSatisfaction / 100.0f; // 0-1
	const int32 BaseAttraction = 10;
	const int32 AttractionScore = FMath::FloorToInt(BaseAttraction * SatisfactionFactor);

	return FMath::Max(1, AttractionScore);
}

void UVisitorSubsystem::UpdateSatisfaction()
{
	// Satisfaction is influenced by multiple factors:
	// - Animal variety and happiness
	// - Cleanliness
	// - Available facilities (food, restrooms)
	// - Crowding (visitor count vs capacity)
	// For now, compute a simple crowding-based satisfaction.

	const float OldSatisfaction = AverageSatisfaction;

	if (CurrentVisitorCount == 0)
	{
		// No visitors, keep neutral
		AverageSatisfaction = 50.0f;
	}
	else
	{
		const float CrowdingRatio = static_cast<float>(CurrentVisitorCount) / static_cast<float>(MaxVisitors);
		// Satisfaction decreases as crowding increases beyond 70% capacity
		float CrowdingPenalty = 0.0f;
		if (CrowdingRatio > 0.7f)
		{
			CrowdingPenalty = (CrowdingRatio - 0.7f) * 100.0f; // up to 30 penalty at full capacity
		}

		AverageSatisfaction = FMath::Clamp(75.0f - CrowdingPenalty, 0.0f, 100.0f);
	}

	if (!FMath::IsNearlyEqual(OldSatisfaction, AverageSatisfaction, 0.1f))
	{
		OnSatisfactionChanged.Broadcast(AverageSatisfaction);
	}
}

FZooVisitorReport UVisitorSubsystem::GetVisitorReport() const
{
	FZooVisitorReport Report;
	Report.CurrentCount = CurrentVisitorCount;
	Report.MaxCapacity = MaxVisitors;
	Report.AverageSatisfaction = AverageSatisfaction;
	Report.AttractionScore = CalculateVisitorAttraction();

	return Report;
}
