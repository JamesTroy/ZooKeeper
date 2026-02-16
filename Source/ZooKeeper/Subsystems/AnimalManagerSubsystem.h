#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnimalManagerSubsystem.generated.h"

class AAnimalBase;
class AEnclosureActor;

/** Broadcast when an animal is registered with the manager. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalAdded, AAnimalBase*, Animal);

/** Broadcast when an animal is unregistered from the manager. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalRemoved, AAnimalBase*, Animal);

/**
 * UAnimalManagerSubsystem
 *
 * World subsystem that maintains a registry of all animals currently in the zoo.
 * Provides lookup and spawning utilities for animal actors.
 */
UCLASS(meta = (DisplayName = "Animal Manager Subsystem"))
class ZOOKEEPER_API UAnimalManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Registration
	// -------------------------------------------------------------------

	/** Registers an animal with the manager. Called by the animal on BeginPlay. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Animals")
	void RegisterAnimal(AAnimalBase* Animal);

	/** Unregisters an animal from the manager. Called by the animal on EndPlay. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Animals")
	void UnregisterAnimal(AAnimalBase* Animal);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns all animals assigned to the given enclosure. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animals")
	TArray<AAnimalBase*> GetAnimalsInEnclosure(AEnclosureActor* Enclosure) const;

	/** Returns the total number of registered animals. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Animals")
	int32 GetAnimalCount() const;

	// -------------------------------------------------------------------
	//  Spawning
	// -------------------------------------------------------------------

	/**
	 * Spawns an animal of the given class at the specified transform and assigns it to an enclosure.
	 * @param AnimalClass   The class of animal to spawn.
	 * @param SpawnTransform  Where to place the animal.
	 * @param Enclosure     The enclosure to assign (may be nullptr).
	 * @return The spawned animal, or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Animals", meta = (DeterminesOutputType = "AnimalClass"))
	AAnimalBase* SpawnAnimal(TSubclassOf<AAnimalBase> AnimalClass, FTransform SpawnTransform, AEnclosureActor* Enclosure);

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animals")
	FOnAnimalAdded OnAnimalAdded;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animals")
	FOnAnimalRemoved OnAnimalRemoved;

private:
	/** Master list of all animals currently alive in the zoo. */
	UPROPERTY()
	TArray<TObjectPtr<AAnimalBase>> AllAnimals;
};
