#include "TutorialWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UTutorialWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// ---- Tooltip panel (bottom-center) ----
		UVerticalBox* TooltipBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TooltipBox"));
		UCanvasPanelSlot* TooltipSlot = RootCanvas->AddChildToCanvas(TooltipBox);
		TooltipSlot->SetAnchors(FAnchors(0.5f, 0.8f));
		TooltipSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		TooltipSlot->SetSize(FVector2D(500.0f, 150.0f));

		// Step title
		StepTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StepTitle"));
		FSlateFontInfo TitleFont = StepTitleText->GetFont();
		TitleFont.Size = 20;
		StepTitleText->SetFont(TitleFont);
		StepTitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.85f, 0.2f)));
		StepTitleText->SetJustification(ETextJustify::Center);
		StepTitleText->SetText(FText::FromString(TEXT("Tutorial")));
		TooltipBox->AddChildToVerticalBox(StepTitleText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// Step message
		StepMessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StepMessage"));
		FSlateFontInfo MsgFont = TitleFont;
		MsgFont.Size = 14;
		StepMessageText->SetFont(MsgFont);
		StepMessageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		StepMessageText->SetJustification(ETextJustify::Center);
		StepMessageText->SetAutoWrapText(true);
		StepMessageText->SetText(FText::FromString(TEXT("Welcome to Zoo Keeper!")));
		TooltipBox->AddChildToVerticalBox(StepMessageText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// Bottom row: counter + buttons
		UHorizontalBox* BottomRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BottomRow"));
		TooltipBox->AddChildToVerticalBox(BottomRow);

		// Step counter (left)
		StepCounterText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StepCounter"));
		FSlateFontInfo CounterFont = TitleFont;
		CounterFont.Size = 12;
		StepCounterText->SetFont(CounterFont);
		StepCounterText->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f)));
		StepCounterText->SetText(FText::FromString(TEXT("1 / 1")));
		UHorizontalBoxSlot* CounterSlot = BottomRow->AddChildToHorizontalBox(StepCounterText);
		CounterSlot->SetSize(FSlateChildSize(1.0f));
		CounterSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));

		// Next button (center-right)
		UTextBlock* NextText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NextText"));
		NextText->SetFont(CounterFont);
		NextText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		NextText->SetText(FText::FromString(TEXT("[Next]")));
		BottomRow->AddChildToHorizontalBox(NextText)->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

		// Skip button (right)
		UTextBlock* SkipText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SkipText"));
		SkipText->SetFont(CounterFont);
		SkipText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.3f, 0.3f)));
		SkipText->SetText(FText::FromString(TEXT("[Skip]")));
		BottomRow->AddChildToHorizontalBox(SkipText);
	}

	return Super::RebuildWidget();
}

void UTutorialWidget::EnsureDefaultSteps()
{
	if (Steps.Num() > 0)
	{
		return;
	}

	Steps.Add({TEXT("Welcome!"), TEXT("Welcome to Zoo Keeper! Let's learn the basics of running your zoo.")});
	Steps.Add({TEXT("Movement"), TEXT("Use WASD to move around the zoo. Hold Shift to sprint. Use the mouse to look around.")});
	Steps.Add({TEXT("Build Mode"), TEXT("Press B to enter Build Mode. Here you can place enclosures, fences, paths, and decorations.")});
	Steps.Add({TEXT("Placing Buildings"), TEXT("In Build Mode, click an item from the menu, then left-click to place it. Right-click to cancel. Use E/Q to rotate.")});
	Steps.Add({TEXT("Animals"), TEXT("Purchase animals from the Animal Catalog. Each species has unique habitat and food requirements.")});
	Steps.Add({TEXT("Feeding"), TEXT("Animals need food! Place feeders in enclosures and keep them stocked. Hungry animals become unhappy.")});
	Steps.Add({TEXT("Staff"), TEXT("Hire zookeepers to maintain enclosures and feed animals. Veterinarians keep your animals healthy.")});
	Steps.Add({TEXT("Visitors"), TEXT("Happy animals attract more visitors. Visitors pay admission and generate income for your zoo.")});
	Steps.Add({TEXT("Research"), TEXT("Invest in research to unlock new buildings, species, and upgrades. Open the Research tab to get started.")});
	Steps.Add({TEXT("Good Luck!"), TEXT("That's the basics! Build enclosures, care for your animals, and grow your zoo into a 5-star attraction!")});
}

void UTutorialWidget::StartTutorial()
{
	EnsureDefaultSteps();

	CurrentStepIndex = 0;
	bIsComplete = false;
	SetVisibility(ESlateVisibility::Visible);
	DisplayCurrentStep();

	UE_LOG(LogZooKeeper, Log, TEXT("TutorialWidget: Tutorial started with %d steps."), Steps.Num());
}

void UTutorialWidget::NextStep()
{
	CurrentStepIndex++;

	if (CurrentStepIndex >= Steps.Num())
	{
		FinishTutorial();
		return;
	}

	DisplayCurrentStep();
}

void UTutorialWidget::SkipTutorial()
{
	UE_LOG(LogZooKeeper, Log, TEXT("TutorialWidget: Tutorial skipped at step %d/%d."), CurrentStepIndex + 1, Steps.Num());
	FinishTutorial();
}

void UTutorialWidget::DisplayCurrentStep()
{
	if (!Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FTutorialStep& Step = Steps[CurrentStepIndex];

	if (StepTitleText)
	{
		StepTitleText->SetText(FText::FromString(Step.Title));
	}

	if (StepMessageText)
	{
		StepMessageText->SetText(FText::FromString(Step.Message));
	}

	if (StepCounterText)
	{
		StepCounterText->SetText(FText::FromString(
			FString::Printf(TEXT("%d / %d"), CurrentStepIndex + 1, Steps.Num())));
	}
}

void UTutorialWidget::FinishTutorial()
{
	bIsComplete = true;
	SetVisibility(ESlateVisibility::Collapsed);
	OnTutorialCompleted.Broadcast();

	UE_LOG(LogZooKeeper, Log, TEXT("TutorialWidget: Tutorial completed."));
}
