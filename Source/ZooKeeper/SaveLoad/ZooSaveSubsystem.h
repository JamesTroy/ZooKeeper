#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ZooSaveSubsystem.generated.h"

class UZooSaveGame;

/**
 * UZooSaveSubsystem
 *
 * Game instance subsystem that manages save/load operations for the zoo.
 * Persists across level transitions since it lives on the game instance.
 * Uses Unreal's built-in USaveGame serialization via SaveGameToSlot/LoadGameFromSlot.
 */
UCLASS(meta = (DisplayName = "Zoo Save Subsystem"))
class ZOOKEEPER_API UZooSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Save / Load API
	// -------------------------------------------------------------------

	/**
	 * Saves the current game state to the given slot name.
	 * Gathers data from world subsystems and serializes to disk.
	 * @param SlotName  The name of the save slot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	void SaveGame(FString SlotName);

	/**
	 * Loads game state from the given slot name and applies it to the world.
	 * @param SlotName  The name of the save slot to load.
	 * @return true if the load succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	bool LoadGame(FString SlotName);

	/**
	 * Deletes the save data in the given slot.
	 * @param SlotName  The name of the save slot to delete.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	void DeleteSave(FString SlotName);

	/**
	 * Checks whether a save file exists in the given slot.
	 * @param SlotName  The name of the save slot to check.
	 * @return true if a save exists in this slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|SaveLoad")
	bool DoesSaveExist(FString SlotName) const;

	/**
	 * Returns a list of all save slot names that have save data.
	 * Scans a predefined set of slot names.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|SaveLoad")
	TArray<FString> GetAllSaveSlots() const;

private:
	/** The user index for save operations (single-player = 0). */
	static constexpr int32 UserIndex = 0;

	/** Maximum number of save slots to scan when enumerating saves. */
	static constexpr int32 MaxSaveSlots = 10;

	/** Cached pointer to the most recently loaded save game object. */
	UPROPERTY()
	TObjectPtr<UZooSaveGame> CachedSaveGame;
};
