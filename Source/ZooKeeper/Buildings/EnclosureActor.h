#pragma once

#include "CoreMinimal.h"
#include "ZooBuildingActor.h"
#include "EnclosureActor.generated.h"

class AAnimalBase;
class UEnclosureVolumeComponent;

/** Broadcast when an animal is added to this enclosure. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimalAddedToEnclosure, AEnclosureActor*, Enclosure, AAnimalBase*, Animal);

/** Broadcast when an animal is removed from this enclosure. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimalRemovedFromEnclosure, AEnclosureActor*, Enclosure, AAnimalBase*, Animal);

/**
 * AEnclosureActor
 *
 * A building that contains animals within a defined polygon volume.
 * Manages a list of contained animals, enforces capacity limits,
 * and provides spatial queries for animal AI navigation.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Enclosure"))
class ZOOKEEPER_API AEnclosureActor : public AZooBuildingActor
{
	GENERATED_BODY()

public:
	AEnclosureActor();

	// -------------------------------------------------------------------
	//  Components
	// -------------------------------------------------------------------

	/** Volume component defining the enclosure's boundary polygon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Enclosure|Components")
	TObjectPtr<UEnclosureVolumeComponent> EnclosureVolume;

	// -------------------------------------------------------------------
	//  Properties
	// -------------------------------------------------------------------

	/** The biome type this enclosure simulates (e.g. "Savanna", "Arctic", "Tropical"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure|Environment")
	FName BiomeType;

	/** Total walkable area of the enclosure in square unreal units. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Enclosure|Info")
	float EnclosureArea;

	/** Maximum number of animals this enclosure can hold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure|Capacity", meta = (ClampMin = "0"))
	int32 MaxAnimalCapacity;

	/** Animals currently contained within this enclosure. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Enclosure|Animals")
	TArray<TObjectPtr<AAnimalBase>> ContainedAnimals;

	// -------------------------------------------------------------------
	//  Animal Management
	// -------------------------------------------------------------------

	/**
	 * Adds an animal to this enclosure.
	 * @param Animal  The animal to add. Ignored if null, already present, or at capacity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Enclosure|Animals")
	void AddAnimal(AAnimalBase* Animal);

	/**
	 * Removes an animal from this enclosure.
	 * @param Animal  The animal to remove. Ignored if null or not present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Enclosure|Animals")
	void RemoveAnimal(AAnimalBase* Animal);

	/**
	 * Returns the number of animals currently in this enclosure.
	 * @return The animal count.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Animals")
	int32 GetAnimalCount() const;

	/**
	 * Checks whether the enclosure has reached its maximum animal capacity.
	 * @return true if at or over capacity.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Animals")
	bool IsAtCapacity() const;

	/**
	 * Returns a random navigable point within the enclosure boundary.
	 * Delegates to the EnclosureVolumeComponent.
	 * @return A random world-space point inside the enclosure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Enclosure")
	FVector GetRandomPointInEnclosure() const;

	/**
	 * Checks whether this enclosure's biome is suitable for the given species.
	 * @param SpeciesID  The species identifier to check.
	 * @return true if the enclosure is suitable for the species.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Animals")
	bool IsSuitableForSpecies(FName SpeciesID) const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Enclosure|Events")
	FOnAnimalAddedToEnclosure OnAnimalAddedToEnclosure;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Enclosure|Events")
	FOnAnimalRemovedFromEnclosure OnAnimalRemovedFromEnclosure;

protected:
	virtual void BeginPlay() override;
};
