#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingDefinition.generated.h"

class AZooBuildingActor;
class UStaticMesh;
class UTexture2D;

/**
 * EBuildingCategory
 *
 * Categorizes the different types of buildings available in the zoo.
 */
UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
	Enclosure		UMETA(DisplayName = "Enclosure"),
	Path			UMETA(DisplayName = "Path"),
	Decoration		UMETA(DisplayName = "Decoration"),
	Facility		UMETA(DisplayName = "Facility"),
	AnimalShelter	UMETA(DisplayName = "Animal Shelter"),
	FoodStation		UMETA(DisplayName = "Food Station"),
	WaterStation	UMETA(DisplayName = "Water Station")
};

/**
 * UBuildingDefinition
 *
 * Data asset that defines the properties of a building type.
 * Used by the building placement system to determine cost, appearance,
 * footprint, and which actor class to spawn.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Building Definition"))
class ZOOKEEPER_API UBuildingDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Identity
	// -------------------------------------------------------------------

	/** Unique identifier for this building type. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Identity")
	FName BuildingID;

	/** Localized display name shown in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Identity")
	FText DisplayName;

	/** Localized description shown in tooltips. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Identity", meta = (MultiLine = "true"))
	FText Description;

	/** The category this building belongs to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Identity")
	EBuildingCategory Category;

	// -------------------------------------------------------------------
	//  Visuals
	// -------------------------------------------------------------------

	/** Static mesh used for the placement preview ghost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Visuals")
	TObjectPtr<UStaticMesh> PreviewMesh;

	/** Thumbnail icon displayed in the build menu UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Visuals")
	TObjectPtr<UTexture2D> ThumbnailIcon;

	// -------------------------------------------------------------------
	//  Gameplay
	// -------------------------------------------------------------------

	/** The actor class to spawn when this building is placed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Gameplay")
	TSubclassOf<AZooBuildingActor> ActorClass;

	/** One-time cost to place this building. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Economy", meta = (ClampMin = "0"))
	int32 PurchaseCost;

	/** Daily maintenance cost deducted by the economy system. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Economy", meta = (ClampMin = "0"))
	int32 MaintenanceCostPerDay;

	/** Grid footprint of this building in grid cells (X columns, Y rows). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Placement", meta = (ClampMin = "1"))
	FIntPoint GridFootprint = FIntPoint(1, 1);

	// -------------------------------------------------------------------
	//  UPrimaryDataAsset Interface
	// -------------------------------------------------------------------

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("BuildingDefinition", BuildingID);
	}
};
