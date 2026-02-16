#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ZooDataTypes.generated.h"

class USkeletalMesh;
class UAnimBlueprint;

/**
 * EResearchCategory
 *
 * Categorizes research nodes into distinct research branches.
 */
UENUM(BlueprintType)
enum class EResearchCategory : uint8
{
	Animals			UMETA(DisplayName = "Animals"),
	Buildings		UMETA(DisplayName = "Buildings"),
	Economy			UMETA(DisplayName = "Economy"),
	Veterinary		UMETA(DisplayName = "Veterinary"),
	Conservation	UMETA(DisplayName = "Conservation")
};

/**
 * EBiomeType
 *
 * Enumerates the different biome types available in the zoo.
 */
UENUM(BlueprintType)
enum class EBiomeType : uint8
{
	Temperate	UMETA(DisplayName = "Temperate"),
	Tropical	UMETA(DisplayName = "Tropical"),
	Arctic		UMETA(DisplayName = "Arctic"),
	Desert		UMETA(DisplayName = "Desert"),
	Savanna		UMETA(DisplayName = "Savanna"),
	Aquatic		UMETA(DisplayName = "Aquatic")
};

/**
 * FAnimalSpeciesRow
 *
 * Data table row defining the attributes of an animal species.
 * Used to configure species-specific behavior, visuals, needs, and economy values.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FAnimalSpeciesRow : public FTableRowBase
{
	GENERATED_BODY()

	// -------------------------------------------------------------------
	//  Identity
	// -------------------------------------------------------------------

	/** Unique identifier for this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity")
	FName SpeciesID;

	/** Localized display name shown in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity")
	FText DisplayName;

	/** Localized description shown in tooltips and info panels. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity", meta = (MultiLine = "true"))
	FText Description;

	// -------------------------------------------------------------------
	//  Visuals
	// -------------------------------------------------------------------

	/** Skeletal mesh asset for this species. Soft reference for async loading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visuals")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	/** Animation blueprint for this species. Soft reference for async loading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visuals")
	TSoftObjectPtr<UAnimBlueprint> AnimBP;

	// -------------------------------------------------------------------
	//  Habitat
	// -------------------------------------------------------------------

	/** The biome type this species prefers. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat")
	FName PreferredBiome;

	/** Minimum enclosure area (in square units) required for this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat", meta = (ClampMin = "0.0"))
	float MinEnclosureArea = 0.0f;

	/** Maximum number of individuals that can coexist in one enclosure. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat", meta = (ClampMin = "1"))
	int32 MaxGroupSize = 1;

	// -------------------------------------------------------------------
	//  Needs Decay Rates (units per second)
	// -------------------------------------------------------------------

	/** Rate at which hunger decays over time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float HungerDecayRate = 0.01f;

	/** Rate at which thirst decays over time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float ThirstDecayRate = 0.015f;

	/** Rate at which energy decays over time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float EnergyDecayRate = 0.008f;

	/** Rate at which social need decays over time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float SocialDecayRate = 0.005f;

	/** List of food types accepted by this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs")
	TArray<FName> AcceptedFoods;

	// -------------------------------------------------------------------
	//  Economy
	// -------------------------------------------------------------------

	/** One-time cost to purchase this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Economy", meta = (ClampMin = "0"))
	int32 PurchasePrice = 0;

	/** Daily cost to feed one individual of this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Economy", meta = (ClampMin = "0"))
	int32 FeedingCostPerDay = 0;

	// -------------------------------------------------------------------
	//  Visitor Appeal
	// -------------------------------------------------------------------

	/** How attractive this species is to visitors (higher = more visitors). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visitors", meta = (ClampMin = "0.0"))
	float VisitorAttractionScore = 1.0f;

	// -------------------------------------------------------------------
	//  Lifespan
	// -------------------------------------------------------------------

	/** Expected lifespan in game days. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Lifespan", meta = (ClampMin = "0.0"))
	float LifeExpectancy = 365.0f;
};

/**
 * FResearchNodeData
 *
 * Data table row defining a node in the research tech tree.
 * Each node has prerequisites, a cost, and unlocks new species or buildings.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FResearchNodeData : public FTableRowBase
{
	GENERATED_BODY()

	/** Unique identifier for this research node. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	FName ResearchID;

	/** Localized display name shown in the research tree UI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	FText DisplayName;

	/** Localized description explaining the research and its benefits. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity", meta = (MultiLine = "true"))
	FText Description;

	/** Monetary cost to begin this research. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Cost", meta = (ClampMin = "0"))
	int32 ResearchCost = 0;

	/** Duration in game days to complete this research. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Cost", meta = (ClampMin = "0.0"))
	float ResearchDuration = 1.0f;

	/** Research IDs that must be completed before this node becomes available. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Prerequisites")
	TArray<FName> Prerequisites;

	/** The branch/category this research belongs to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	EResearchCategory Category = EResearchCategory::Animals;

	/** Species IDs unlocked upon completing this research. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Unlocks")
	TArray<FName> UnlockedSpecies;

	/** Building IDs unlocked upon completing this research. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Unlocks")
	TArray<FName> UnlockedBuildings;
};

/**
 * FBiomeConfig
 *
 * Data table row defining the configuration for a biome type.
 * Specifies environmental parameters and which species are native to it.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FBiomeConfig : public FTableRowBase
{
	GENERATED_BODY()

	/** Unique identifier for this biome configuration. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	FName BiomeID;

	/** Localized display name for this biome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	FText DisplayName;

	/** The biome type classification. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	EBiomeType Type = EBiomeType::Temperate;

	/** The ideal temperature for this biome in degrees Celsius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Environment")
	float IdealTemperature = 20.0f;

	/** How far from IdealTemperature the biome can deviate before stressing animals. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Environment", meta = (ClampMin = "0.0"))
	float TemperatureTolerance = 10.0f;

	/** Species IDs that are native to this biome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Species")
	TArray<FName> NativeSpecies;
};
