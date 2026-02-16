#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ZooSaveGame.generated.h"

/**
 * FZooAnimalSaveData
 *
 * Serializable snapshot of a single animal's state for save/load.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooAnimalSaveData
{
	GENERATED_BODY()

	/** The species identifier (e.g. "Lion", "Elephant"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FName SpeciesID;

	/** World transform of the animal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Hunger = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Thirst = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Energy = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Health = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Happiness = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Social = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FString AnimalName;
};

/**
 * FZooBuildingSaveData
 *
 * Serializable snapshot of a single building's state for save/load.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooBuildingSaveData
{
	GENERATED_BODY()

	/** The building type identifier (e.g. "FoodStall", "Restroom"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FName BuildingID;

	/** World transform of the building. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FTransform Transform;

	/** Current structural condition (0 = destroyed, 1 = pristine). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Condition = 1.0f;
};

/**
 * FZooStaffSaveData
 *
 * Serializable snapshot of a staff member's state for save/load.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooStaffSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 StaffID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	uint8 Type = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 Salary = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Skill = 0.5f;
};

/**
 * UZooSaveGame
 *
 * USaveGame subclass that holds all persistent zoo state.
 * Used by UZooSaveSubsystem for serialization to/from disk.
 */
UCLASS(meta = (DisplayName = "Zoo Save Game"))
class ZOOKEEPER_API UZooSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UZooSaveGame();

	/** Save format version for migration support. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SaveVersion = 1;

	// --- Core State ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SavedFunds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SavedDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float SavedTimeOfDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SavedSeason;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FString ZooName;

	// --- Entities ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FZooAnimalSaveData> SavedAnimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FZooBuildingSaveData> SavedBuildings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FZooStaffSaveData> SavedStaff;

	// --- Research & Milestones ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FName> CompletedResearch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FName CurrentResearchID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float CurrentResearchProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FName> AchievedMilestones;

	// --- Weather ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	uint8 SavedWeatherState;

	// --- Economy ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 LoanBalance;

	// --- Player ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FTransform PlayerTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	uint8 ActiveToolIndex;
};
