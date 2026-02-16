#include "PauseMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "SaveLoad/ZooSaveSubsystem.h"
#include "Subsystems/TimeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UPauseMenuWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UVerticalBox* MenuLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MenuLayout"));
		UCanvasPanelSlot* MenuSlot = RootCanvas->AddChildToCanvas(MenuLayout);
		MenuSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		MenuSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		MenuSlot->SetAutoSize(true);

		// Title
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("PAUSED")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 28;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TitleText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(TitleText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 20.0f));

		// Menu items
		const TArray<FString> MenuItems = {
			TEXT("[Resume]"), TEXT("[Save Game]"), TEXT("[Load Game]"), TEXT("[Quit to Desktop]")
		};

		FSlateFontInfo ItemFont = TitleFont;
		ItemFont.Size = 18;

		for (int32 i = 0; i < MenuItems.Num(); ++i)
		{
			UTextBlock* ItemText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
				*FString::Printf(TEXT("MenuItem_%d"), i));
			ItemText->SetText(FText::FromString(MenuItems[i]));
			ItemText->SetFont(ItemFont);
			ItemText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
			ItemText->SetJustification(ETextJustify::Center);
			MenuLayout->AddChildToVerticalBox(ItemText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		}
	}

	return Super::RebuildWidget();
}

void UPauseMenuWidget::OnResumeClicked()
{
	// Unpause and hide widget
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->ResumeTime();
		}
	}

	SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogZooKeeper, Log, TEXT("PauseMenuWidget: Resumed."));
}

void UPauseMenuWidget::OnSaveClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			SaveSub->SaveGame(TEXT("Quicksave"));
			UE_LOG(LogZooKeeper, Log, TEXT("PauseMenuWidget: Quick saved."));
		}
	}
}

void UPauseMenuWidget::OnLoadClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			if (SaveSub->LoadGame(TEXT("Quicksave")))
			{
				SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogZooKeeper, Log, TEXT("PauseMenuWidget: Quick loaded."));
			}
		}
	}
}

void UPauseMenuWidget::OnQuitClicked()
{
	UE_LOG(LogZooKeeper, Log, TEXT("PauseMenuWidget: Quit requested."));
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
