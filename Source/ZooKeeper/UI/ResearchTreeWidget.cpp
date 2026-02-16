#include "ResearchTreeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UResearchTreeWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UTextBlock* Placeholder = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Placeholder"));
		Placeholder->SetText(FText::FromString(TEXT("[Research Tree - Coming Soon]")));
		FSlateFontInfo Font = Placeholder->GetFont();
		Font.Size = 18;
		Placeholder->SetFont(Font);
		Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Placeholder);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);

		ResearchNodesPanel = nullptr;
		CurrentResearchText = nullptr;
		ResearchProgressBar = nullptr;
	}

	return Super::RebuildWidget();
}

void UResearchTreeWidget::RefreshResearchTree()
{
	if (!ResearchNodesPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchTreeWidget: ResearchNodesPanel is not bound."));
		return;
	}

	ResearchNodesPanel->ClearChildren();

	UpdateResearchProgress();

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchTreeWidget: Research tree refreshed."));
}

void UResearchTreeWidget::StartResearchClicked(FName ResearchID)
{
	if (ResearchID.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchTreeWidget: Attempted to start research with an empty ID."));
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchTreeWidget: Start research clicked for '%s'."), *ResearchID.ToString());

	if (CurrentResearchText)
	{
		CurrentResearchText->SetText(FText::FromName(ResearchID));
	}

	if (ResearchProgressBar)
	{
		ResearchProgressBar->SetPercent(0.0f);
	}
}

void UResearchTreeWidget::UpdateResearchProgress()
{
	if (CurrentResearchText)
	{
		CurrentResearchText->SetText(FText::FromString(TEXT("No Research Active")));
	}

	if (ResearchProgressBar)
	{
		ResearchProgressBar->SetPercent(0.0f);
	}
}
