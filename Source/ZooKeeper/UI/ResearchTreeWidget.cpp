#include "ResearchTreeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystems/ResearchSubsystem.h"
#include "Data/ZooDataTypes.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UResearchTreeWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// Main layout
		UVerticalBox* MainLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
		UCanvasPanelSlot* MainSlot = RootCanvas->AddChildToCanvas(MainLayout);
		MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		MainSlot->SetOffsets(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

		// --- Title ---
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("RESEARCH TREE")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 20;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UVerticalBoxSlot* TitleSlot = MainLayout->AddChildToVerticalBox(TitleText);
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// --- Current Research Status Bar ---
		UHorizontalBox* StatusBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("StatusBar"));
		UVerticalBoxSlot* StatusSlot = MainLayout->AddChildToVerticalBox(StatusBar);
		StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		UTextBlock* StatusLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StatusLabel"));
		StatusLabel->SetText(FText::FromString(TEXT("Researching: ")));
		FSlateFontInfo LabelFont = StatusLabel->GetFont();
		LabelFont.Size = 14;
		StatusLabel->SetFont(LabelFont);
		StatusLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		UHorizontalBoxSlot* StatusLabelSlot = StatusBar->AddChildToHorizontalBox(StatusLabel);
		StatusLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 4.0f, 0.0f));

		CurrentResearchText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CurrentResearchText"));
		CurrentResearchText->SetText(FText::FromString(TEXT("None")));
		CurrentResearchText->SetFont(LabelFont);
		CurrentResearchText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.8f, 0.2f)));
		StatusBar->AddChildToHorizontalBox(CurrentResearchText);

		// --- Progress Bar ---
		ResearchProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("ResearchProgressBar"));
		ResearchProgressBar->SetPercent(0.0f);
		ResearchProgressBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.6f, 1.0f));
		UVerticalBoxSlot* ProgressSlot = MainLayout->AddChildToVerticalBox(ResearchProgressBar);
		ProgressSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// --- Available Research Header ---
		UTextBlock* AvailHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AvailHeader"));
		AvailHeader->SetText(FText::FromString(TEXT("Available Research:")));
		FSlateFontInfo HeaderFont = AvailHeader->GetFont();
		HeaderFont.Size = 14;
		AvailHeader->SetFont(HeaderFont);
		AvailHeader->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		UVerticalBoxSlot* AvailHeaderSlot = MainLayout->AddChildToVerticalBox(AvailHeader);
		AvailHeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		// --- Research Nodes ScrollBox ---
		UScrollBox* NodesScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("NodesScroll"));
		UVerticalBoxSlot* ScrollSlot = MainLayout->AddChildToVerticalBox(NodesScroll);
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ResearchNodesPanel = NodesScroll;

		// --- Completed Header ---
		UTextBlock* CompletedHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CompletedHeader"));
		CompletedHeader->SetText(FText::FromString(TEXT("Completed:")));
		CompletedHeader->SetFont(HeaderFont);
		CompletedHeader->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		UVerticalBoxSlot* CompHeaderSlot = MainLayout->AddChildToVerticalBox(CompletedHeader);
		CompHeaderSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 4.0f));
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

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UResearchSubsystem* ResearchSub = World->GetSubsystem<UResearchSubsystem>();
	if (!ResearchSub)
	{
		return;
	}

	// Populate available research nodes
	TArray<FName> Available = ResearchSub->GetAvailableResearch();
	if (Available.Num() == 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyResearch"));
		EmptyText->SetText(FText::FromString(TEXT("No research available.")));
		FSlateFontInfo EmptyFont = EmptyText->GetFont();
		EmptyFont.Size = 11;
		EmptyText->SetFont(EmptyFont);
		EmptyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		ResearchNodesPanel->AddChild(EmptyText);
	}
	else
	{
		int32 NodeIndex = 0;
		for (const FName& TopicID : Available)
		{
			FString DisplayStr = FString::Printf(TEXT("  [Start] %s"), *TopicID.ToString());

			// Try to get display name from DataTable
			if (ResearchSub->ResearchDataTable)
			{
				if (const FResearchNodeData* Row = ResearchSub->ResearchDataTable->FindRow<FResearchNodeData>(TopicID, TEXT("RefreshUI")))
				{
					DisplayStr = FString::Printf(TEXT("  [Start] %s  |  %.0fs  |  $%d"),
						*Row->DisplayName.ToString(), Row->ResearchDuration, Row->ResearchCost);
				}
			}

			UTextBlock* NodeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
				*FString::Printf(TEXT("ResearchNode_%d"), NodeIndex++));
			NodeText->SetText(FText::FromString(DisplayStr));
			FSlateFontInfo NodeFont = NodeText->GetFont();
			NodeFont.Size = 12;
			NodeText->SetFont(NodeFont);
			NodeText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
			ResearchNodesPanel->AddChild(NodeText);
		}
	}

	UpdateResearchProgress();

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchTreeWidget: Refreshed with %d available topics."), Available.Num());
}

void UResearchTreeWidget::StartResearchClicked(FName ResearchID)
{
	if (ResearchID.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchTreeWidget: Attempted to start research with an empty ID."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UResearchSubsystem* ResearchSub = World->GetSubsystem<UResearchSubsystem>();
	if (ResearchSub)
	{
		ResearchSub->StartResearch(ResearchID);
	}

	RefreshResearchTree();
}

void UResearchTreeWidget::UpdateResearchProgress()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UResearchSubsystem* ResearchSub = World->GetSubsystem<UResearchSubsystem>();

	if (CurrentResearchText)
	{
		if (ResearchSub && ResearchSub->GetCurrentResearchID() != NAME_None)
		{
			CurrentResearchText->SetText(FText::FromString(ResearchSub->GetCurrentResearchID().ToString()));
		}
		else
		{
			CurrentResearchText->SetText(FText::FromString(TEXT("None")));
		}
	}

	if (ResearchProgressBar)
	{
		float Progress = ResearchSub ? ResearchSub->GetCurrentResearchProgress() : 0.0f;
		ResearchProgressBar->SetPercent(Progress);
	}
}
