#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ZooDataTypes.generated.h"

class USkeletalMesh;
class UAnimBlueprint;
class UBehaviorTree;
class USoundBase;
class UTexture2D;

// ===================================================================
//  Enums
// ===================================================================

UENUM(BlueprintType)
enum class EResearchCategory : uint8
{
	Animals			UMETA(DisplayName = "Animals"),
	Buildings		UMETA(DisplayName = "Buildings"),
	Economy			UMETA(DisplayName = "Economy"),
	Veterinary		UMETA(DisplayName = "Veterinary"),
	Conservation	UMETA(DisplayName = "Conservation")
};

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

UENUM(BlueprintType)
enum class EFoodType : uint8
{
	Meat	UMETA(DisplayName = "Meat"),
	Fish	UMETA(DisplayName = "Fish"),
	Fruit	UMETA(DisplayName = "Fruit"),
	Grain	UMETA(DisplayName = "Grain"),
	Insects	UMETA(DisplayName = "Insects")
};

UENUM(BlueprintType)
enum class ESocialNeed : uint8
{
	Pack		UMETA(DisplayName = "Pack"),
	Solitary	UMETA(DisplayName = "Solitary"),
	Pair		UMETA(DisplayName = "Pair")
};

UENUM(BlueprintType)
enum class EAnimalSize : uint8
{
	Small	UMETA(DisplayName = "Small"),
	Medium	UMETA(DisplayName = "Medium"),
	Large	UMETA(DisplayName = "Large"),
	Huge	UMETA(DisplayName = "Huge")
};

UENUM(BlueprintType)
enum class EDangerLevel : uint8
{
	Safe		UMETA(DisplayName = "Safe"),
	Moderate	UMETA(DisplayName = "Moderate"),
	Dangerous	UMETA(DisplayName = "Dangerous")
};

// ===================================================================
//  Data Table Row Structs
// ===================================================================

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity")
	FName SpeciesID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Identity", meta = (MultiLine = "true"))
	FText Description;

	// -------------------------------------------------------------------
	//  Classification
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Classification")
	EFoodType FoodType = EFoodType::Meat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Classification")
	ESocialNeed SocialNeed = ESocialNeed::Pack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Classification")
	EAnimalSize Size = EAnimalSize::Medium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Classification")
	EDangerLevel DangerLevel = EDangerLevel::Safe;

	// -------------------------------------------------------------------
	//  Visuals
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visuals")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visuals")
	TSoftObjectPtr<UAnimBlueprint> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visuals")
	TSoftObjectPtr<UTexture2D> Icon;

	// -------------------------------------------------------------------
	//  Audio
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Audio")
	TSoftObjectPtr<USoundBase> SoundCue_Ambient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Audio")
	TSoftObjectPtr<USoundBase> SoundCue_Happy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Audio")
	TSoftObjectPtr<USoundBase> SoundCue_Hungry;

	// -------------------------------------------------------------------
	//  Habitat
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat")
	FName PreferredBiome;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat")
	FVector2D PreferredTemperature = FVector2D(15.0f, 30.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat", meta = (ClampMin = "0.0"))
	float MinEnclosureArea = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Habitat", meta = (ClampMin = "1"))
	int32 MaxGroupSize = 1;

	// -------------------------------------------------------------------
	//  Movement
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Movement", meta = (ClampMin = "0.0"))
	float WalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Movement", meta = (ClampMin = "0.0"))
	float RunSpeed = 600.0f;

	// -------------------------------------------------------------------
	//  Needs Decay Rates (units per second)
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float HungerDecayRate = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float ThirstDecayRate = 0.015f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float EnergyDecayRate = 0.008f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float SocialDecayRate = 0.005f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float HappinessDecayRate = 0.002f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs", meta = (ClampMin = "0.0"))
	float HealthDecayRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Needs")
	TArray<FName> AcceptedFoods;

	// -------------------------------------------------------------------
	//  Enrichment
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Enrichment")
	FGameplayTagContainer EnrichmentTags;

	// -------------------------------------------------------------------
	//  AI
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|AI")
	TSoftObjectPtr<UBehaviorTree> BehaviorTree;

	// -------------------------------------------------------------------
	//  Economy
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Economy", meta = (ClampMin = "0"))
	int32 PurchasePrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Economy", meta = (ClampMin = "0"))
	int32 FeedingCostPerDay = 0;

	// -------------------------------------------------------------------
	//  Visitor Appeal
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Visitors", meta = (ClampMin = "0.0"))
	float VisitorAttractionScore = 1.0f;

	// -------------------------------------------------------------------
	//  Lifespan
	// -------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Species|Lifespan", meta = (ClampMin = "0.0"))
	float LifeExpectancy = 365.0f;
};

// ===================================================================
//  FFoodTypeRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FFoodTypeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Food")
	FName FoodID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Food")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Food", meta = (ClampMin = "0"))
	int32 CostPerUnit = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Food", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HungerRestore = 0.3f;
};

// ===================================================================
//  FEnclosureTemplateRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FEnclosureTemplateRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure")
	FName TemplateID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure")
	EBiomeType BiomeType = EBiomeType::Temperate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure")
	EAnimalSize Size = EAnimalSize::Medium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure")
	FVector2D Dimensions = FVector2D(1000.0f, 1000.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure", meta = (ClampMin = "0"))
	int32 BaseCost = 5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure", meta = (ClampMin = "0"))
	int32 MaintenanceCostPerDay = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure", meta = (ClampMin = "1"))
	int32 MaxCapacity = 5;
};

// ===================================================================
//  FDecorationRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FDecorationRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration")
	FName DecoID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration")
	FName Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration", meta = (ClampMin = "0"))
	int32 Cost = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration", meta = (ClampMin = "0.0"))
	float VisitorBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Decoration")
	TSoftObjectPtr<UStaticMesh> Mesh;
};

// ===================================================================
//  FEnrichmentItemRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FEnrichmentItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment", meta = (ClampMin = "0"))
	int32 Cost = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment", meta = (ClampMin = "0.0"))
	float HappinessBoost = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	TArray<FName> CompatibleSpecies;
};

// ===================================================================
//  FStaffTypeRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FStaffTypeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	FName StaffTypeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0"))
	int32 HireCost = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0"))
	int32 SalaryPerDay = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	TArray<FName> Tasks;
};

// ===================================================================
//  FVisitorTypeRow
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FVisitorTypeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors")
	FName VisitorTypeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0.0"))
	float SpawnWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0"))
	int32 Admission = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0.0"))
	float WalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0.0"))
	float Patience = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0.0"))
	float StayDuration = 600.0f;
};

// ===================================================================
//  FResearchNodeData
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FResearchNodeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	FName ResearchID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Cost", meta = (ClampMin = "0"))
	int32 ResearchCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Cost", meta = (ClampMin = "0.0"))
	float ResearchDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Prerequisites")
	TArray<FName> Prerequisites;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity")
	EResearchCategory Category = EResearchCategory::Animals;

	/** Research tier (0 = starting, 1-3 = progressively advanced). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Identity", meta = (ClampMin = "0", ClampMax = "3"))
	int32 Tier = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Unlocks")
	TArray<FName> UnlockedSpecies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research|Unlocks")
	TArray<FName> UnlockedBuildings;
};

// ===================================================================
//  FBiomeConfig
// ===================================================================

USTRUCT(BlueprintType)
struct ZOOKEEPER_API FBiomeConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	FName BiomeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Identity")
	EBiomeType Type = EBiomeType::Temperate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Environment")
	float IdealTemperature = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Environment", meta = (ClampMin = "0.0"))
	float TemperatureTolerance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Biome|Species")
	TArray<FName> NativeSpecies;
};
