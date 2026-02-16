#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadWidget.generated.h"

class UTextBlock;
class UVerticalBox;

/**
 * USaveLoadWidget
 *
 * Widget that displays save slots and allows the player to save, load, or delete.
 * Supports Quicksave, Autosave, and 5 manual save slots.
 * Builds its widget tree entirely in C++ -- no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Save Load Widget"))
class ZOOKEEPER_API USaveLoadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Opens the widget in save mode. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OpenForSave();

	/** Opens the widget in load mode. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OpenForLoad();

	/** Performs save or load on the given slot. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void SelectSlot(const FString& SlotName);

	/** Deletes the given slot. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void DeleteSlot(const FString& SlotName);

	/** Closes the widget. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void CloseWidget();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	/** Refreshes all slot entries with current save state. */
	void RefreshSlots();

	/** Creates a single slot row widget and adds it to the slot list. */
	void CreateSlotEntry(const FString& SlotName, const FString& DisplayLabel);

	/** Whether we are in save mode (true) or load mode (false). */
	bool bIsSaveMode = false;

	/** Title text block (changes between "Save Game" and "Load Game"). */
	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	/** Container for slot entry rows. */
	UPROPERTY()
	TObjectPtr<UVerticalBox> SlotListBox;

	/** All slot names in display order. */
	static const TArray<FString>& GetSlotNames();
};
