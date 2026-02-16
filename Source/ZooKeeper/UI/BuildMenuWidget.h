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
 * When a building is selected, its details (name, cost, description) are
 * shown and the selection is forwarded to the placement system.
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_BuildMenu).
 */
UCLASS(meta = (DisplayName = "Build Menu Widget"))
class ZOOKEEPER_API UBuildMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Scroll box / panel containing the list of available buildings. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|BuildMenu")
	UPanelWidget* BuildingListPanel;

	/** Displays the name of the current building category. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|BuildMenu")
	UTextBlock* CategoryTitleText;

	/** Displays the name of the currently selected building. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|BuildMenu")
	UTextBlock* SelectedBuildingNameText;

	/** Displays the cost of the currently selected building. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|BuildMenu")
	UTextBlock* SelectedBuildingCostText;

	/** Displays the description of the currently selected building. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|BuildMenu")
	UTextBlock* SelectedBuildingDescText;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** The currently active building category filter. */
	UPROPERTY(BlueprintReadOnly, Category = "Zoo|BuildMenu")
	EBuildingCategory CurrentCategory;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/**
	 * Filters the building list to show only buildings of the given category.
	 * @param Category  The category to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void ShowCategory(EBuildingCategory Category);

	/**
	 * Selects a building, displaying its details and setting it on the placement component.
	 * @param Definition  The building definition to select.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void SelectBuilding(UBuildingDefinition* Definition);

	/** Refreshes the building list based on the current category. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void RefreshBuildingList();

	/** Shows the build menu widget. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void Show();

	/** Hides the build menu widget. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|BuildMenu")
	void Hide();

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	/** Fired when the player selects a building from the menu. */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|BuildMenu")
	FOnBuildingSelected OnBuildingSelected;

private:
	/** The currently selected building definition. */
	UPROPERTY()
	TObjectPtr<UBuildingDefinition> SelectedDefinition;
};
