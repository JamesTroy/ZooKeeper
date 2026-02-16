#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingManagerSubsystem.generated.h"

class AZooBuildingActor;
class AEnclosureActor;

/** Broadcast when a building is placed in the zoo. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingPlaced, AZooBuildingActor*, Building);

/** Broadcast when a building is demolished. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingDemolished, AZooBuildingActor*, Building);

/** Broadcast when a new enclosure boundary is formed. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnclosureFormed, AEnclosureActor*, Enclosure);

/**
 * UBuildingManagerSubsystem
 *
 * World subsystem that tracks all buildings and enclosures placed in the zoo.
 * Provides placement, demolition, and spatial lookup utilities.
 */
UCLASS(meta = (DisplayName = "Building Manager Subsystem"))
class ZOOKEEPER_API UBuildingManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Building Registration
	// -------------------------------------------------------------------

	/** Registers a building with the manager. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Buildings")
	void RegisterBuilding(AZooBuildingActor* Building);

	/** Unregisters a building from the manager. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Buildings")
	void UnregisterBuilding(AZooBuildingActor* Building);

	// -------------------------------------------------------------------
	//  Enclosure Registration
	// -------------------------------------------------------------------

	/** Registers an enclosure with the manager. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Buildings")
	void RegisterEnclosure(AEnclosureActor* Enclosure);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns all registered enclosures. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Buildings")
	TArray<AEnclosureActor*> GetAllEnclosures() const;

	/**
	 * Finds the enclosure whose bounds contain the given world location.
	 * @param Location  The world-space position to query.
	 * @return The enclosure at that location, or nullptr if none.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Buildings")
	AEnclosureActor* FindEnclosureAtLocation(FVector Location) const;

	// -------------------------------------------------------------------
	//  Placement & Demolition
	// -------------------------------------------------------------------

	/**
	 * Spawns and places a building of the given class at the specified transform.
	 * @param BuildingClass   The class of building to spawn.
	 * @param SpawnTransform  Where to place the building.
	 * @return The placed building, or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Buildings", meta = (DeterminesOutputType = "BuildingClass"))
	AZooBuildingActor* PlaceBuilding(TSubclassOf<AZooBuildingActor> BuildingClass, FTransform SpawnTransform);

	/**
	 * Demolishes the given building and removes it from the world.
	 * @param Building  The building to demolish.
	 * @return true if demolition succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Buildings")
	bool DemolishBuilding(AZooBuildingActor* Building);

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Buildings")
	FOnBuildingPlaced OnBuildingPlaced;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Buildings")
	FOnBuildingDemolished OnBuildingDemolished;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Buildings")
	FOnEnclosureFormed OnEnclosureFormed;

private:
	/** All buildings currently placed in the zoo. */
	UPROPERTY()
	TArray<TObjectPtr<AZooBuildingActor>> AllBuildings;

	/** All enclosures currently in the zoo. */
	UPROPERTY()
	TArray<TObjectPtr<AEnclosureActor>> AllEnclosures;
};
