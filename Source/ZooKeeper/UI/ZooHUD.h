#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZooHUD.generated.h"

class UZooHUDWidget;
class UAnimalInfoWidget;
class UBuildMenuWidget;
class UFinancePanelWidget;
class UStaffRosterWidget;
class UResearchTreeWidget;
class UZooOverviewWidget;
class AAnimalBase;

/**
 * AZooHUD
 *
 * Custom HUD class that creates and manages all UI widgets programmatically.
 * Acts as the central UI orchestrator for the Zoo Keeper game.
 */
UCLASS()
class ZOOKEEPER_API AZooHUD : public AHUD
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	// -------------------------------------------------------------------
	//  Public API
	// -------------------------------------------------------------------

	/** Shows the animal info panel for the given animal. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ShowAnimalInfo(AAnimalBase* Animal);

	/** Hides the animal info panel. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void HideAnimalInfo();

	/** Toggles the build menu visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ToggleBuildMenu();

	/** Toggles the finance panel visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ToggleFinancePanel();

	/** Toggles the staff roster visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ToggleStaffRoster();

	/** Toggles the research tree visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ToggleResearchTree();

	/** Toggles the zoo overview visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ToggleZooOverview();

	// -------------------------------------------------------------------
	//  Widget Accessors
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UZooHUDWidget> MainHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UAnimalInfoWidget> AnimalInfoPanel;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UBuildMenuWidget> BuildMenuPanel;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UFinancePanelWidget> FinancePanel;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UStaffRosterWidget> StaffRosterPanel;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UResearchTreeWidget> ResearchTreePanel;

	UPROPERTY(BlueprintReadOnly, Category = "Zoo|HUD")
	TObjectPtr<UZooOverviewWidget> ZooOverviewPanel;

private:
	/** Helper to toggle visibility of a panel widget. */
	void TogglePanelVisibility(UUserWidget* Panel);
};
