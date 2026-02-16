#include "SaveLoadWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Blueprint/WidgetTree.h"
#include "SaveLoad/ZooSaveSubsystem.h"
#include "SaveLoad/ZooSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ZooKeeper.h"

const TArray<FString>& USaveLoadWidget::GetSlotNames()
{
	static const TArray<FString> Names = {
		TEXT("Quicksave"),
		TEXT("Autosave"),
		TEXT("Save_1"),
		TEXT("Save_2"),
		TEXT("Save_3"),
		TEXT("Save_4"),
		TEXT("Save_5")
	};
	return Names;
}

TSharedRef<SWidget> USaveLoadWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// ---- Main panel (centered, takes up most of screen) ----
		UVerticalBox* PanelLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PanelLayout"));
		UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelLayout);
		PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PanelSlot->SetSize(FVector2D(500.0f, 500.0f));

		// Title
		TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("Save Game")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 28;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TitleText->SetJustification(ETextJustify::Center);
		PanelLayout->AddChildToVerticalBox(TitleText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));

		// Scrollable slot list
		UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("SlotScroll"));
		PanelLayout->AddChildToVerticalBox(ScrollBox)->SetSize(FSlateChildSize(1.0f));

		SlotListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SlotListBox"));
		ScrollBox->AddChild(SlotListBox);

		// Close button (bottom)
		UTextBlock* CloseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseText"));
		CloseText->SetText(FText::FromString(TEXT("[Close]")));
		FSlateFontInfo CloseFont = TitleFont;
		CloseFont.Size = 18;
		CloseText->SetFont(CloseFont);
		CloseText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.3f, 0.3f)));
		CloseText->SetJustification(ETextJustify::Center);
		PanelLayout->AddChildToVerticalBox(CloseText)->SetPadding(FMargin(0.0f, 16.0f, 0.0f, 0.0f));
	}

	return Super::RebuildWidget();
}

void USaveLoadWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshSlots();
}

void USaveLoadWidget::OpenForSave()
{
	bIsSaveMode = true;
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("Save Game")));
	}
	SetVisibility(ESlateVisibility::Visible);
	RefreshSlots();

	UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Opened for Save."));
}

void USaveLoadWidget::OpenForLoad()
{
	bIsSaveMode = false;
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("Load Game")));
	}
	SetVisibility(ESlateVisibility::Visible);
	RefreshSlots();

	UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Opened for Load."));
}

void USaveLoadWidget::RefreshSlots()
{
	if (!SlotListBox || !WidgetTree)
	{
		return;
	}

	SlotListBox->ClearChildren();

	const TArray<FString>& Slots = GetSlotNames();
	const TArray<FString> DisplayLabels = {
		TEXT("Quicksave"),
		TEXT("Autosave"),
		TEXT("Slot 1"),
		TEXT("Slot 2"),
		TEXT("Slot 3"),
		TEXT("Slot 4"),
		TEXT("Slot 5")
	};

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		CreateSlotEntry(Slots[i], DisplayLabels[i]);
	}
}

void USaveLoadWidget::CreateSlotEntry(const FString& SlotName, const FString& DisplayLabel)
{
	if (!SlotListBox || !WidgetTree)
	{
		return;
	}

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(),
		*FString::Printf(TEXT("SlotRow_%s"), *SlotName));

	FSlateFontInfo SlotFont;
	SlotFont.Size = 14;

	// Slot label
	UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("SlotLabel_%s"), *SlotName));
	LabelText->SetFont(SlotFont);
	LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	// Check if save exists
	bool bSaveExists = false;
	FString InfoStr;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			bSaveExists = SaveSub->DoesSaveExist(SlotName);
		}
	}

	if (bSaveExists)
	{
		LabelText->SetText(FText::FromString(FString::Printf(TEXT("%s  [Occupied]"), *DisplayLabel)));
	}
	else
	{
		LabelText->SetText(FText::FromString(FString::Printf(TEXT("%s  [Empty]"), *DisplayLabel)));
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
	}

	UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(LabelText);
	LabelSlot->SetSize(FSlateChildSize(1.0f));
	LabelSlot->SetPadding(FMargin(0.0f, 4.0f, 8.0f, 4.0f));

	// Action text (Save/Load)
	UTextBlock* ActionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("SlotAction_%s"), *SlotName));
	ActionText->SetFont(SlotFont);

	if (bIsSaveMode)
	{
		ActionText->SetText(FText::FromString(TEXT("[Save]")));
		ActionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		// Quicksave and Autosave are read-only in save mode (system-managed)
		if (SlotName == TEXT("Quicksave") || SlotName == TEXT("Autosave"))
		{
			ActionText->SetText(FText::FromString(TEXT("--")));
			ActionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.4f)));
		}
	}
	else
	{
		if (bSaveExists)
		{
			ActionText->SetText(FText::FromString(TEXT("[Load]")));
			ActionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.7f, 1.0f)));
		}
		else
		{
			ActionText->SetText(FText::FromString(TEXT("--")));
			ActionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.4f)));
		}
	}

	Row->AddChildToHorizontalBox(ActionText)->SetPadding(FMargin(0.0f, 4.0f, 8.0f, 4.0f));

	// Delete text
	UTextBlock* DeleteText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("SlotDelete_%s"), *SlotName));
	DeleteText->SetFont(SlotFont);

	if (bSaveExists)
	{
		DeleteText->SetText(FText::FromString(TEXT("[Del]")));
		DeleteText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.3f, 0.3f)));
	}
	else
	{
		DeleteText->SetText(FText::FromString(TEXT("")));
	}

	Row->AddChildToHorizontalBox(DeleteText)->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 4.0f));

	SlotListBox->AddChildToVerticalBox(Row)->SetPadding(FMargin(0.0f, 2.0f, 0.0f, 2.0f));
}

void USaveLoadWidget::SelectSlot(const FString& SlotName)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>();
	if (!SaveSub)
	{
		return;
	}

	if (bIsSaveMode)
	{
		SaveSub->SaveGame(SlotName);
		UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Saved to slot '%s'."), *SlotName);
	}
	else
	{
		if (SaveSub->LoadGame(SlotName))
		{
			UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Loaded from slot '%s'."), *SlotName);
			CloseWidget();
			return;
		}
		else
		{
			UE_LOG(LogZooKeeper, Warning, TEXT("SaveLoadWidget: Failed to load slot '%s'."), *SlotName);
		}
	}

	RefreshSlots();
}

void USaveLoadWidget::DeleteSlot(const FString& SlotName)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			SaveSub->DeleteSave(SlotName);
			UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Deleted slot '%s'."), *SlotName);
		}
	}

	RefreshSlots();
}

void USaveLoadWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogZooKeeper, Log, TEXT("SaveLoadWidget: Closed."));
}
