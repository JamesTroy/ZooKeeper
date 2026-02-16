#include "MainMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "SaveLoad/ZooSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UMainMenuWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// ---- Title (top-center) ----
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("ZOO KEEPER")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 48;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.85f, 0.3f)));
		TitleText->SetJustification(ETextJustify::Center);

		UCanvasPanelSlot* TitleSlot = RootCanvas->AddChildToCanvas(TitleText);
		TitleSlot->SetAnchors(FAnchors(0.5f, 0.2f));
		TitleSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		TitleSlot->SetAutoSize(true);

		// ---- Subtitle ----
		UTextBlock* SubtitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SubtitleText"));
		SubtitleText->SetText(FText::FromString(TEXT("Build your dream zoo")));
		FSlateFontInfo SubFont = TitleFont;
		SubFont.Size = 16;
		SubtitleText->SetFont(SubFont);
		SubtitleText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
		SubtitleText->SetJustification(ETextJustify::Center);

		UCanvasPanelSlot* SubSlot = RootCanvas->AddChildToCanvas(SubtitleText);
		SubSlot->SetAnchors(FAnchors(0.5f, 0.28f));
		SubSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		SubSlot->SetAutoSize(true);

		// ---- Menu layout (centered) ----
		UVerticalBox* MenuLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MenuLayout"));
		UCanvasPanelSlot* MenuSlot = RootCanvas->AddChildToCanvas(MenuLayout);
		MenuSlot->SetAnchors(FAnchors(0.5f, 0.55f));
		MenuSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		MenuSlot->SetAutoSize(true);

		FSlateFontInfo ItemFont = TitleFont;
		ItemFont.Size = 22;

		// New Game
		UTextBlock* NewGameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NewGameText"));
		NewGameText->SetText(FText::FromString(TEXT("[New Game]")));
		NewGameText->SetFont(ItemFont);
		NewGameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		NewGameText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(NewGameText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// Continue
		ContinueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContinueText"));
		ContinueText->SetText(FText::FromString(TEXT("[Continue]")));
		ContinueText->SetFont(ItemFont);
		ContinueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		ContinueText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(ContinueText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// Load Game
		UTextBlock* LoadText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LoadText"));
		LoadText->SetText(FText::FromString(TEXT("[Load Game]")));
		LoadText->SetFont(ItemFont);
		LoadText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		LoadText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(LoadText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// Settings
		UTextBlock* SettingsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SettingsText"));
		SettingsText->SetText(FText::FromString(TEXT("[Settings]")));
		SettingsText->SetFont(ItemFont);
		SettingsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
		SettingsText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(SettingsText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// Quit
		UTextBlock* QuitText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QuitText"));
		QuitText->SetText(FText::FromString(TEXT("[Quit]")));
		QuitText->SetFont(ItemFont);
		QuitText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.3f, 0.3f)));
		QuitText->SetJustification(ETextJustify::Center);
		MenuLayout->AddChildToVerticalBox(QuitText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// ---- Version text (bottom-right) ----
		UTextBlock* VersionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("VersionText"));
		VersionText->SetText(FText::FromString(TEXT("v0.1 - Early Development")));
		FSlateFontInfo VerFont = TitleFont;
		VerFont.Size = 10;
		VersionText->SetFont(VerFont);
		VersionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.4f)));

		UCanvasPanelSlot* VerSlot = RootCanvas->AddChildToCanvas(VersionText);
		VerSlot->SetAnchors(FAnchors(1.0f, 1.0f));
		VerSlot->SetAlignment(FVector2D(1.0f, 1.0f));
		VerSlot->SetAutoSize(true);
		VerSlot->SetOffsets(FMargin(-16.0f, -16.0f, 16.0f, 16.0f));
	}

	return Super::RebuildWidget();
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide Continue if no quicksave/autosave exists
	if (ContinueText)
	{
		bool bHasSave = false;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
			{
				bHasSave = SaveSub->DoesSaveExist(TEXT("Quicksave")) || SaveSub->DoesSaveExist(TEXT("Autosave"));
			}
		}

		ContinueText->SetVisibility(bHasSave ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UMainMenuWidget::OnNewGameClicked()
{
	UE_LOG(LogZooKeeper, Log, TEXT("MainMenuWidget: New Game."));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("ZooMap"));
}

void UMainMenuWidget::OnContinueClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			// Try quicksave first, then autosave
			if (SaveSub->DoesSaveExist(TEXT("Quicksave")))
			{
				SaveSub->LoadGame(TEXT("Quicksave"));
			}
			else if (SaveSub->DoesSaveExist(TEXT("Autosave")))
			{
				SaveSub->LoadGame(TEXT("Autosave"));
			}
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("MainMenuWidget: Continue."));
}

void UMainMenuWidget::OnLoadGameClicked()
{
	UE_LOG(LogZooKeeper, Log, TEXT("MainMenuWidget: Load Game requested. Open SaveLoadWidget."));
	// The SaveLoadWidget should be opened by the owning HUD/controller
}

void UMainMenuWidget::OnSettingsClicked()
{
	UE_LOG(LogZooKeeper, Log, TEXT("MainMenuWidget: Settings requested."));
	// Settings panel TBD
}

void UMainMenuWidget::OnQuitClicked()
{
	UE_LOG(LogZooKeeper, Log, TEXT("MainMenuWidget: Quit."));
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
