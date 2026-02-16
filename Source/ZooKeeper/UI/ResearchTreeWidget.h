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
 * nodes, the currently active research topic, and its progress.
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Research Tree Widget"))
class ZOOKEEPER_API UResearchTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void RefreshResearchTree();

	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void StartResearchClicked(FName ResearchID);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void UpdateResearchProgress();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TObjectPtr<UPanelWidget> ResearchNodesPanel;

	UPROPERTY()
	TObjectPtr<UTextBlock> CurrentResearchText;

	UPROPERTY()
	TObjectPtr<UProgressBar> ResearchProgressBar;
};
