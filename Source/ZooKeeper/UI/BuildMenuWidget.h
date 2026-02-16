#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/BuildingDefinition.h"
#include "BuildMenuWidget.generated.h"

class UTextBlock;
class UPanelWidget;
class UBuildingDefinition;

/** Broadcast when the player selects a building from the build menu. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingSelected, UBuildingDefinition*, SelectedDefinition);

/**
 * UBuildMenuWidget
 *
 * Widget for the building placement menu. Displays buildings filtered by
 * category and allows the player to select a building for placement.
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Build Menu Widget"))
class ZOOKEEPER_API UBuildMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** The currently active building category filter. */
	UPROPERTY(BlueprintReadOnly, Category = "Zoo|BuildMenu")
	EBuildingCategory CurrentCategory;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void ShowCategory(EBuildingCategory Category);

	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void SelectBuilding(UBuildingDefinition* Definition);

	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void RefreshBuildingList();

	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void Hide();

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|BuildMenu")
	FOnBuildingSelected OnBuildingSelected;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TObjectPtr<UPanelWidget> BuildingListPanel;

	UPROPERTY()
	TObjectPtr<UTextBlock> CategoryTitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedBuildingNameText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedBuildingCostText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedBuildingDescText;

	UPROPERTY()
	TObjectPtr<UBuildingDefinition> SelectedDefinition;
};
