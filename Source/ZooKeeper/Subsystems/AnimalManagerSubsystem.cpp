#include "AnimalManagerSubsystem.h"
#include "Animals/AnimalBase.h"
#include "ZooKeeper.h"
#include "Engine/World.h"

bool UAnimalManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UAnimalManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogZooKeeper, Log, TEXT("AnimalManagerSubsystem::Initialize"));
}

void UAnimalManagerSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("AnimalManagerSubsystem::Deinitialize - %d animals registered at shutdown."), AllAnimals.Num());

	AllAnimals.Empty();

	Super::Deinitialize();
}

void UAnimalManagerSubsystem::RegisterAnimal(AAnimalBase* Animal)
{
	if (!Animal)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalManagerSubsystem::RegisterAnimal - Null animal passed."));
		return;
	}

	if (AllAnimals.Contains(Animal))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalManagerSubsystem::RegisterAnimal - Animal already registered."));
		return;
	}

	AllAnimals.Add(Animal);
	OnAnimalAdded.Broadcast(Animal);

	UE_LOG(LogZooKeeper, Log, TEXT("AnimalManagerSubsystem - Animal registered. Total: %d"), AllAnimals.Num());
}

void UAnimalManagerSubsystem::UnregisterAnimal(AAnimalBase* Animal)
{
	if (!Animal)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalManagerSubsystem::UnregisterAnimal - Null animal passed."));
		return;
	}

	const int32 Removed = AllAnimals.Remove(Animal);
	if (Removed > 0)
	{
		OnAnimalRemoved.Broadcast(Animal);

		UE_LOG(LogZooKeeper, Log, TEXT("AnimalManagerSubsystem - Animal unregistered. Total: %d"), AllAnimals.Num());
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalManagerSubsystem::UnregisterAnimal - Animal was not registered."));
	}
}

TArray<AAnimalBase*> UAnimalManagerSubsystem::GetAnimalsInEnclosure(AEnclosureActor* Enclosure) const
{
	TArray<AAnimalBase*> Result;

	for (AAnimalBase* Animal : AllAnimals)
	{
		if (!Animal)
		{
			continue;
		}

		if (!Enclosure || Animal->CurrentEnclosure == Enclosure)
		{
			Result.Add(Animal);
		}
	}

	return Result;
}

int32 UAnimalManagerSubsystem::GetAnimalCount() const
{
	return AllAnimals.Num();
}

AAnimalBase* UAnimalManagerSubsystem::SpawnAnimal(TSubclassOf<AAnimalBase> AnimalClass, FTransform SpawnTransform, AEnclosureActor* Enclosure)
{
	if (!AnimalClass)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalManagerSubsystem::SpawnAnimal - Null animal class."));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("AnimalManagerSubsystem::SpawnAnimal - No world available."));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAnimalBase* NewAnimal = World->SpawnActor<AAnimalBase>(AnimalClass, SpawnTransform, SpawnParams);
	if (NewAnimal)
	{
		// Registration will happen via the animal's BeginPlay calling RegisterAnimal,
		// but we also register here in case it doesn't.
		if (!AllAnimals.Contains(NewAnimal))
		{
			RegisterAnimal(NewAnimal);
		}

		UE_LOG(LogZooKeeper, Log, TEXT("AnimalManagerSubsystem - Spawned animal of class %s."), *AnimalClass->GetName());
	}
	else
	{
		UE_LOG(LogZooKeeper, Error, TEXT("AnimalManagerSubsystem::SpawnAnimal - Failed to spawn animal."));
	}

	return NewAnimal;
}
