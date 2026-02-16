#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZooGameInstance.generated.h"

class UZooSaveGame;

/**
 * UZooGameInstance
 *
 * Persistent game instance that survives level transitions. Manages
 * save/load operations and holds the current save slot reference.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Game Instance"))
class ZOOKEEPER_API UZooGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UZooGameInstance();

	//~ Begin UGameInstance Interface
	virtual void Init() override;
	//~ End UGameInstance Interface

	// ---------------------------------------------------------------
	//  Save / Load
	// ---------------------------------------------------------------

	/**
	 * Saves the current game state to the active save slot.
	 * @return true if the save operation was initiated successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	bool SaveGame();

	/**
	 * Loads a game from the specified save slot.
	 * @param SlotName  The name of the save slot to load.
	 * @return true if the load operation was initiated successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	bool LoadGame(const FString& SlotName);

	/** Returns the name of the currently active save slot. */
	UFUNCTION(BlueprintPure, Category = "Zoo|SaveLoad")
	FString GetCurrentSaveSlot() const { return CurrentSaveSlot; }

	/** Sets the active save slot name. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
	void SetCurrentSaveSlot(const FString& SlotName);

protected:
	/** The name of the currently active save slot. */
	UPROPERTY(BlueprintReadOnly, Category = "Zoo|SaveLoad", meta = (AllowPrivateAccess = "true"))
	FString CurrentSaveSlot;

	/** User index for save operations (0 = first local player). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|SaveLoad")
	int32 SaveUserIndex;
};
