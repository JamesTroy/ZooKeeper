#include "EnclosureActor.h"
#include "EnclosureVolumeComponent.h"
#include "ZooKeeper/ZooKeeper.h"
#include "ZooKeeper/Subsystems/BuildingManagerSubsystem.h"
#include "ZooKeeper/Subsystems/AnimalManagerSubsystem.h"
#include "ZooKeeper/Data/ZooDataTypes.h"
#include "Engine/DataTable.h"

AEnclosureActor::AEnclosureActor()
	: EnclosureArea(0.0f)
	, MaxAnimalCapacity(5)
{
	// Create the enclosure volume component
	EnclosureVolume = CreateDefaultSubobject<UEnclosureVolumeComponent>(TEXT("EnclosureVolume"));
	EnclosureVolume->SetupAttachment(BuildingMesh);
}

void AEnclosureActor::BeginPlay()
{
	Super::BeginPlay();

	// Calculate the initial area from the volume component
	if (EnclosureVolume)
	{
		EnclosureArea = EnclosureVolume->CalculateArea();
	}

	// Register specifically as an enclosure with the BuildingManagerSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UBuildingManagerSubsystem* BuildingManager = World->GetSubsystem<UBuildingManagerSubsystem>())
		{
			BuildingManager->RegisterEnclosure(this);
			UE_LOG(LogZooKeeper, Log, TEXT("Enclosure '%s' registered as enclosure with BuildingManagerSubsystem (Biome: %s, Area: %.1f)."),
				*BuildingName, *BiomeType.ToString(), EnclosureArea);
		}
	}
}

void AEnclosureActor::AddAnimal(AAnimalBase* Animal)
{
	if (!Animal)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': Attempted to add null animal."), *BuildingName);
		return;
	}

	if (ContainedAnimals.Contains(Animal))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': Animal already present."), *BuildingName);
		return;
	}

	if (IsAtCapacity())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': At capacity (%d/%d), cannot add animal."),
			*BuildingName, ContainedAnimals.Num(), MaxAnimalCapacity);
		return;
	}

	ContainedAnimals.Add(Animal);
	OnAnimalAddedToEnclosure.Broadcast(this, Animal);

	UE_LOG(LogZooKeeper, Log, TEXT("Enclosure '%s': Animal added (%d/%d)."),
		*BuildingName, ContainedAnimals.Num(), MaxAnimalCapacity);
}

void AEnclosureActor::RemoveAnimal(AAnimalBase* Animal)
{
	if (!Animal)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': Attempted to remove null animal."), *BuildingName);
		return;
	}

	const int32 RemovedCount = ContainedAnimals.Remove(Animal);
	if (RemovedCount > 0)
	{
		OnAnimalRemovedFromEnclosure.Broadcast(this, Animal);
		UE_LOG(LogZooKeeper, Log, TEXT("Enclosure '%s': Animal removed (%d/%d)."),
			*BuildingName, ContainedAnimals.Num(), MaxAnimalCapacity);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': Animal not found for removal."), *BuildingName);
	}
}

int32 AEnclosureActor::GetAnimalCount() const
{
	return ContainedAnimals.Num();
}

bool AEnclosureActor::IsAtCapacity() const
{
	return ContainedAnimals.Num() >= MaxAnimalCapacity;
}

FVector AEnclosureActor::GetRandomPointInEnclosure() const
{
	if (EnclosureVolume)
	{
		return EnclosureVolume->GetRandomPointInside();
	}

	UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': No EnclosureVolumeComponent, returning actor location."), *BuildingName);
	return GetActorLocation();
}

bool AEnclosureActor::IsSuitableForSpecies(FName SpeciesID) const
{
	if (BiomeType.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': No biome type set, species suitability check failed."), *BuildingName);
		return false;
	}

	// Look up the species data from the AnimalManagerSubsystem's DataTable.
	if (UWorld* World = GetWorld())
	{
		if (UAnimalManagerSubsystem* AnimalManager = World->GetSubsystem<UAnimalManagerSubsystem>())
		{
			if (UDataTable* SpeciesTable = AnimalManager->SpeciesDataTable)
			{
				FAnimalSpeciesRow* SpeciesRow = SpeciesTable->FindRow<FAnimalSpeciesRow>(SpeciesID, TEXT("IsSuitableForSpecies"));
				if (SpeciesRow)
				{
					// Compare the enclosure's biome type against the species' preferred biome.
					if (SpeciesRow->PreferredBiome != BiomeType)
					{
						UE_LOG(LogZooKeeper, Log, TEXT("Enclosure '%s' (Biome: %s) not suitable for species '%s' (Preferred: %s)."),
							*BuildingName, *BiomeType.ToString(), *SpeciesID.ToString(), *SpeciesRow->PreferredBiome.ToString());
						return false;
					}
					return true;
				}
				else
				{
					UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': Species '%s' not found in DataTable."),
						*BuildingName, *SpeciesID.ToString());
				}
			}
		}
	}

	// Fallback: allow if we can't look up the species data.
	return true;
}
