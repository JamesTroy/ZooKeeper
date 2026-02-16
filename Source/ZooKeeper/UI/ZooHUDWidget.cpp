#include "ZooHUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/Text/STextBlock.h"
#include "Subsystems/TimeSubsystem.h"
#include "Subsystems/EconomySubsystem.h"
#include "Interaction/InteractionComponent.h"
#include "Core/ZooKeeperCharacter.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UZooHUDWidget::RebuildWidget()
{
	// DIAGNOSTIC: Return a raw Slate text to verify widget is reaching the screen.
	// If this text appears, the issue is in WidgetTree construction.
	// If it doesn't, the issue is in widget creation/viewport.
	return SNew(STextBlock)
		.Text(FText::FromString(TEXT("=== HUD TEST ===")))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 36))
		.ColorAndOpacity(FLinearColor::Yellow);
}

void UZooHUDWidget::BuildWidgetTree()
{
	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	// -- Top-left: Time + Day vertical box --
	UVerticalBox* TimeBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TimeBox"));
	UCanvasPanelSlot* TimeBoxSlot = RootCanvas->AddChildToCanvas(TimeBox);
	TimeBoxSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	TimeBoxSlot->SetAlignment(FVector2D(0.0f, 0.0f));
	TimeBoxSlot->SetPosition(FVector2D(20.0f, 20.0f));
	TimeBoxSlot->SetAutoSize(true);

	TimeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TimeText"));
	TimeText->SetText(FText::FromString(TEXT("00:00")));
	FSlateFontInfo TimeFont = TimeText->GetFont();
	TimeFont.Size = 24;
	TimeText->SetFont(TimeFont);
	TimeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	UVerticalBoxSlot* TimeTextSlot = TimeBox->AddChildToVerticalBox(TimeText);
	TimeTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);

	DayText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DayText"));
	DayText->SetText(FText::FromString(TEXT("Day 1")));
	FSlateFontInfo DayFont = DayText->GetFont();
	DayFont.Size = 18;
	DayText->SetFont(DayFont);
	DayText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	UVerticalBoxSlot* DayTextSlot = TimeBox->AddChildToVerticalBox(DayText);
	DayTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);

	// -- Top-right: Funds text --
	FundsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FundsText"));
	FundsText->SetText(FText::FromString(TEXT("$0")));
	FSlateFontInfo FundsFont = FundsText->GetFont();
	FundsFont.Size = 22;
	FundsText->SetFont(FundsFont);
	FundsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.9f, 0.2f)));
	UCanvasPanelSlot* FundsSlot = RootCanvas->AddChildToCanvas(FundsText);
	FundsSlot->SetAnchors(FAnchors(1.0f, 0.0f));
	FundsSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	FundsSlot->SetPosition(FVector2D(-20.0f, 20.0f));
	FundsSlot->SetAutoSize(true);

	// -- Center: Crosshair (4x4 white dot) --
	CrosshairImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImage"));
	CrosshairImage->SetColorAndOpacity(FLinearColor::White);
	CrosshairImage->SetDesiredSizeOverride(FVector2D(4.0f, 4.0f));
	UCanvasPanelSlot* CrosshairSlot = RootCanvas->AddChildToCanvas(CrosshairImage);
	CrosshairSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	CrosshairSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CrosshairSlot->SetPosition(FVector2D(0.0f, 0.0f));
	CrosshairSlot->SetSize(FVector2D(4.0f, 4.0f));

	// -- Bottom-center (75%): Interaction prompt --
	InteractionPromptText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InteractionPromptText"));
	InteractionPromptText->SetText(FText::GetEmpty());
	FSlateFontInfo PromptFont = InteractionPromptText->GetFont();
	PromptFont.Size = 18;
	InteractionPromptText->SetFont(PromptFont);
	InteractionPromptText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	UCanvasPanelSlot* PromptSlot = RootCanvas->AddChildToCanvas(InteractionPromptText);
	PromptSlot->SetAnchors(FAnchors(0.5f, 0.75f));
	PromptSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	PromptSlot->SetPosition(FVector2D(0.0f, 0.0f));
	PromptSlot->SetAutoSize(true);

	UE_LOG(LogZooKeeper, Log, TEXT("ZooHUDWidget: Widget tree built programmatically."));
}

void UZooHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// --- Bind to Subsystems (world is available now) ---
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: No world available during NativeConstruct."));
		return;
	}

	UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>();
	if (TimeSys)
	{
		TimeSubsystem = TimeSys;
		TimeSys->OnTimeOfDayChanged.AddDynamic(this, &UZooHUDWidget::HandleTimeOfDayChanged);
		TimeSys->OnDayChanged.AddDynamic(this, &UZooHUDWidget::HandleDayChanged);

		UpdateTimeDisplay(TimeSys->GetFormattedTime());
		UpdateDayDisplay(FText::FromString(FString::Printf(TEXT("Day %d"), TimeSys->CurrentDay)));
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: TimeSubsystem not found."));
	}

	UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>();
	if (EconSys)
	{
		EconomySubsystem = EconSys;
		EconSys->OnFundsChanged.AddDynamic(this, &UZooHUDWidget::HandleFundsChanged);

		UpdateFundsDisplay(EconSys->GetBalance());
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: EconomySubsystem not found."));
	}

	HideInteractionPrompt();
}

void UZooHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedInteractionComp.IsValid())
	{
		if (const APlayerController* PC = GetOwningPlayer())
		{
			if (const AZooKeeperCharacter* Character = Cast<AZooKeeperCharacter>(PC->GetPawn()))
			{
				CachedInteractionComp = Character->GetInteractionComponent();
			}
		}
	}

	if (CachedInteractionComp.IsValid())
	{
		UInteractionComponent* InteractionComp = CachedInteractionComp.Get();
		if (InteractionComp->HasInteractable())
		{
			ShowInteractionPrompt(InteractionComp->GetFocusedInteractionPrompt());
		}
		else
		{
			HideInteractionPrompt();
		}
	}
}

void UZooHUDWidget::UpdateTimeDisplay(FText Time)
{
	if (TimeText)
	{
		TimeText->SetText(Time);
	}
}

void UZooHUDWidget::UpdateDayDisplay(FText Day)
{
	if (DayText)
	{
		DayText->SetText(Day);
	}
}

void UZooHUDWidget::UpdateFundsDisplay(int32 Funds)
{
	if (FundsText)
	{
		FundsText->SetText(FText::FromString(FString::Printf(TEXT("$%d"), Funds)));
	}
}

void UZooHUDWidget::ShowInteractionPrompt(FText Prompt)
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetText(Prompt);
		InteractionPromptText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UZooHUDWidget::HideInteractionPrompt()
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UZooHUDWidget::SetCrosshairVisible(bool bVisible)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UZooHUDWidget::HandleTimeOfDayChanged(float NewTime)
{
	if (TimeSubsystem.IsValid())
	{
		UpdateTimeDisplay(TimeSubsystem->GetFormattedTime());
	}
}

void UZooHUDWidget::HandleDayChanged(int32 NewDay)
{
	UpdateDayDisplay(FText::FromString(FString::Printf(TEXT("Day %d"), NewDay)));
}

void UZooHUDWidget::HandleFundsChanged(int32 NewBalance)
{
	UpdateFundsDisplay(NewBalance);
}
