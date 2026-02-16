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

	/** Current hunger level (0 = full, 1 = starving). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Hunger = 0.0f;

	/** Current thirst level (0 = quenched, 1 = dehydrated). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Thirst = 0.0f;

	/** Current health (0 = dead, 1 = perfect health). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Health = 1.0f;

	/** Current happiness (0 = miserable, 1 = ecstatic). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float Happiness = 0.5f;

	/** The animal's player-given name. */
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

	/** The zoo's fund balance at the time of saving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SavedFunds;

	/** The in-game day number at the time of saving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	int32 SavedDay;

	/** The time of day (0-24 float) at the time of saving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	float SavedTimeOfDay;

	/** The player-given name for the zoo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	FString ZooName;

	/** Saved state for every animal in the zoo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FZooAnimalSaveData> SavedAnimals;

	/** Saved state for every building in the zoo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|SaveLoad")
	TArray<FZooBuildingSaveData> SavedBuildings;
};
