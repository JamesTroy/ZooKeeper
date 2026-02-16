#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResearchTreeWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UPanelWidget;

/**
 * UResearchTreeWidget
 *
 * Widget that displays the zoo's research tree, showing available research
 * nodes, the currently active research topic, and its progress. Players can
 * click a research node to begin researching it.
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_ResearchTree).
 */
UCLASS(meta = (DisplayName = "Research Tree Widget"))
class ZOOKEEPER_API UResearchTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Panel containing all research node widgets. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Research")
	UPanelWidget* ResearchNodesPanel;

	/** Displays the name of the currently active research topic. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Research")
	UTextBlock* CurrentResearchText;

	/** Shows the progress of the currently active research (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Research")
	UProgressBar* ResearchProgressBar;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/** Rebuilds the research node display from the research subsystem. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void RefreshResearchTree();

	/**
	 * Begins researching the topic with the given ID.
	 * @param ResearchID  The unique name identifier of the research topic.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void StartResearchClicked(FName ResearchID);

	/** Updates the research progress bar from the current research state. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void UpdateResearchProgress();
};
