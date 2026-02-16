#include "BuildingManagerSubsystem.h"
#include "Buildings/ZooBuildingActor.h"
#include "Buildings/EnclosureActor.h"
#include "ZooKeeper.h"
#include "Engine/World.h"

bool UBuildingManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UBuildingManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem::Initialize"));
}

void UBuildingManagerSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem::Deinitialize - %d buildings, %d enclosures at shutdown."),
		AllBuildings.Num(), AllEnclosures.Num());

	AllBuildings.Empty();
	AllEnclosures.Empty();

	Super::Deinitialize();
}

void UBuildingManagerSubsystem::RegisterBuilding(AZooBuildingActor* Building)
{
	if (!Building)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::RegisterBuilding - Null building passed."));
		return;
	}

	if (AllBuildings.Contains(Building))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::RegisterBuilding - Building already registered."));
		return;
	}

	AllBuildings.Add(Building);

	UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem - Building registered. Total: %d"), AllBuildings.Num());
}

void UBuildingManagerSubsystem::UnregisterBuilding(AZooBuildingActor* Building)
{
	if (!Building)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::UnregisterBuilding - Null building passed."));
		return;
	}

	const int32 Removed = AllBuildings.Remove(Building);
	if (Removed > 0)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem - Building unregistered. Total: %d"), AllBuildings.Num());
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::UnregisterBuilding - Building was not registered."));
	}
}

void UBuildingManagerSubsystem::RegisterEnclosure(AEnclosureActor* Enclosure)
{
	if (!Enclosure)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::RegisterEnclosure - Null enclosure passed."));
		return;
	}

	if (AllEnclosures.Contains(Enclosure))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::RegisterEnclosure - Enclosure already registered."));
		return;
	}

	AllEnclosures.Add(Enclosure);
	OnEnclosureFormed.Broadcast(Enclosure);

	UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem - Enclosure registered. Total: %d"), AllEnclosures.Num());
}

AEnclosureActor* UBuildingManagerSubsystem::FindEnclosureAtLocation(FVector Location) const
{
	// Iterate through all enclosures and check if the location is within their bounds.
	// This relies on AEnclosureActor having a bounding volume. When AEnclosureActor is
	// fully defined, this should use its GetComponentsBoundingBox or a custom containment check.
	for (AEnclosureActor* Enclosure : AllEnclosures)
	{
		if (!Enclosure)
		{
			continue;
		}

		// Use the actor's bounding box for a basic containment test.
		FBox BoundingBox;
		FVector Origin;
		FVector Extent;
		static_cast<AActor*>(Enclosure)->GetActorBounds(false, Origin, Extent);
		BoundingBox = FBox(Origin - Extent, Origin + Extent);

		if (BoundingBox.IsInsideOrOn(Location))
		{
			return Enclosure;
		}
	}

	return nullptr;
}

AZooBuildingActor* UBuildingManagerSubsystem::PlaceBuilding(TSubclassOf<AZooBuildingActor> BuildingClass, FTransform SpawnTransform)
{
	if (!BuildingClass)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::PlaceBuilding - Null building class."));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("BuildingManagerSubsystem::PlaceBuilding - No world available."));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AZooBuildingActor* NewBuilding = World->SpawnActor<AZooBuildingActor>(BuildingClass, SpawnTransform, SpawnParams);
	if (NewBuilding)
	{
		if (!AllBuildings.Contains(NewBuilding))
		{
			RegisterBuilding(NewBuilding);
		}

		OnBuildingPlaced.Broadcast(NewBuilding);

		UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem - Placed building of class %s."), *BuildingClass->GetName());
	}
	else
	{
		UE_LOG(LogZooKeeper, Error, TEXT("BuildingManagerSubsystem::PlaceBuilding - Failed to spawn building."));
	}

	return NewBuilding;
}

bool UBuildingManagerSubsystem::DemolishBuilding(AZooBuildingActor* Building)
{
	if (!Building)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::DemolishBuilding - Null building passed."));
		return false;
	}

	if (!AllBuildings.Contains(Building))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingManagerSubsystem::DemolishBuilding - Building not registered."));
		return false;
	}

	AllBuildings.Remove(Building);
	OnBuildingDemolished.Broadcast(Building);

	// Destroy the actor from the world
	Building->Destroy();

	UE_LOG(LogZooKeeper, Log, TEXT("BuildingManagerSubsystem - Building demolished. Total: %d"), AllBuildings.Num());

	return true;
}
