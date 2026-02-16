#include "VisitorSubsystem.h"
#include "ZooRatingSubsystem.h"
#include "Visitors/VisitorCharacter.h"
#include "Kismet/GameplayStatics.h"
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

	// Find all actors tagged as visitor spawn points.
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsWithTag(World, FName(TEXT("VisitorSpawn")), SpawnPoints);
		UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem::Initialize - Found %d visitor spawn points."), SpawnPoints.Num());
	}

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

	UWorld* World = GetWorld();
	if (!World || !VisitorCharacterClass)
	{
		// Fallback: just increment counter if no class assigned.
		CurrentVisitorCount += ActualSpawn;
		OnVisitorCountChanged.Broadcast(CurrentVisitorCount);
		UE_LOG(LogZooKeeper, Warning, TEXT("VisitorSubsystem::SpawnVisitors - No VisitorCharacterClass set, incrementing counter only."));
		return;
	}

	int32 Spawned = 0;
	for (int32 i = 0; i < ActualSpawn; ++i)
	{
		FVector SpawnLocation = FVector::ZeroVector;
		FRotator SpawnRotation = FRotator::ZeroRotator;

		// Pick a random spawn point if available.
		if (SpawnPoints.Num() > 0)
		{
			AActor* SpawnPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
			if (SpawnPoint)
			{
				SpawnLocation = SpawnPoint->GetActorLocation();
				SpawnRotation = SpawnPoint->GetActorRotation();
			}
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AVisitorCharacter* NewVisitor = World->SpawnActor<AVisitorCharacter>(
			VisitorCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (NewVisitor)
		{
			Spawned++;
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Spawned %d/%d visitor actors. Total: %d"),
		Spawned, ActualSpawn, CurrentVisitorCount);
}

void UVisitorSubsystem::DespawnAllVisitors()
{
	if (AllVisitorCharacters.Num() == 0 && CurrentVisitorCount == 0)
	{
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("VisitorSubsystem - Despawning all %d visitors."), AllVisitorCharacters.Num());

	// Destroy all visitor actors. Copy the array since Destroy triggers EndPlay -> UnregisterVisitor.
	TArray<TObjectPtr<AVisitorCharacter>> VisitorsCopy = AllVisitorCharacters;
	for (AVisitorCharacter* Visitor : VisitorsCopy)
	{
		if (Visitor)
		{
			Visitor->Destroy();
		}
	}

	AllVisitorCharacters.Empty();
	CurrentVisitorCount = 0;
	OnVisitorCountChanged.Broadcast(CurrentVisitorCount);
}

int32 UVisitorSubsystem::CalculateVisitorAttraction() const
{
	const float SatisfactionFactor = AverageSatisfaction / 100.0f; // 0-1
	const int32 BaseAttraction = 10;

	// Use zoo rating to boost attraction if ZooRatingSubsystem is available
	float RatingMultiplier = 1.0f;
	if (const UWorld* World = GetWorld())
	{
		if (const UZooRatingSubsystem* RatingSub = World->GetSubsystem<UZooRatingSubsystem>())
		{
			RatingMultiplier = RatingSub->GetVisitorSpawnMultiplier();
		}
	}

	const int32 AttractionScore = FMath::FloorToInt(BaseAttraction * SatisfactionFactor * RatingMultiplier);
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
