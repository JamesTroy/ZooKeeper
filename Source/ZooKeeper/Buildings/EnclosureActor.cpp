#include "EnclosureActor.h"
#include "EnclosureVolumeComponent.h"
#include "ZooKeeper/ZooKeeper.h"
#include "ZooKeeper/Subsystems/BuildingManagerSubsystem.h"

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
	// TODO: Implement species-to-biome mapping via a data table or species definition.
	// For now, any enclosure with a valid biome type is considered suitable.
	if (BiomeType.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Enclosure '%s': No biome type set, species suitability check failed."), *BuildingName);
		return false;
	}

	// Placeholder: all non-empty biome types are suitable for all species.
	// This will be replaced with a proper lookup when the species data system is implemented.
	return true;
}
