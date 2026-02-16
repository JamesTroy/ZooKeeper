#include "BuildMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Data/BuildingDefinition.h"
#include "Engine/AssetManager.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UBuildMenuWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		// Root canvas for positioning
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// Main vertical layout
		UVerticalBox* MainLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
		UCanvasPanelSlot* MainSlot = RootCanvas->AddChildToCanvas(MainLayout);
		MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		MainSlot->SetOffsets(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

		// --- Title ---
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("BUILD MENU")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 20;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UVerticalBoxSlot* TitleSlot = MainLayout->AddChildToVerticalBox(TitleText);
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// --- Category Tabs (horizontal row of labels) ---
		UHorizontalBox* CategoryBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CategoryBar"));
		UVerticalBoxSlot* CategoryBarSlot = MainLayout->AddChildToVerticalBox(CategoryBar);
		CategoryBarSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// Add category labels
		const TArray<TPair<FString, EBuildingCategory>> Categories = {
			{TEXT("Enclosures"), EBuildingCategory::Enclosure},
			{TEXT("Paths"), EBuildingCategory::Path},
			{TEXT("Decorations"), EBuildingCategory::Decoration},
			{TEXT("Facilities"), EBuildingCategory::Facility},
			{TEXT("Food"), EBuildingCategory::FoodStation},
			{TEXT("Shelters"), EBuildingCategory::AnimalShelter}
		};

		for (const auto& Cat : Categories)
		{
			UTextBlock* CatLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
				*FString::Printf(TEXT("Cat_%s"), *Cat.Key));
			CatLabel->SetText(FText::FromString(FString::Printf(TEXT("[%s]"), *Cat.Key)));
			FSlateFontInfo CatFont = CatLabel->GetFont();
			CatFont.Size = 12;
			CatLabel->SetFont(CatFont);
			CatLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
			UHorizontalBoxSlot* CatSlot = CategoryBar->AddChildToHorizontalBox(CatLabel);
			CatSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
		}

		// --- Category Title ---
		CategoryTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CategoryTitle"));
		CategoryTitleText->SetText(FText::FromString(TEXT("Enclosures")));
		FSlateFontInfo CatTitleFont = CategoryTitleText->GetFont();
		CatTitleFont.Size = 16;
		CategoryTitleText->SetFont(CatTitleFont);
		CategoryTitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.9f, 0.4f)));
		UVerticalBoxSlot* CatTitleSlot = MainLayout->AddChildToVerticalBox(CategoryTitleText);
		CatTitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		// --- Building List (ScrollBox) ---
		UScrollBox* BuildingScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("BuildingScroll"));
		UVerticalBoxSlot* ScrollSlot = MainLayout->AddChildToVerticalBox(BuildingScroll);
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		ScrollSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		BuildingListPanel = BuildingScroll;

		// --- Selected Building Detail Section ---
		UVerticalBox* DetailBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DetailBox"));
		UVerticalBoxSlot* DetailSlot = MainLayout->AddChildToVerticalBox(DetailBox);
		DetailSlot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 0.0f));

		// Selected building name
		SelectedBuildingNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedName"));
		SelectedBuildingNameText->SetText(FText::FromString(TEXT("No building selected")));
		FSlateFontInfo NameFont = SelectedBuildingNameText->GetFont();
		NameFont.Size = 14;
		SelectedBuildingNameText->SetFont(NameFont);
		SelectedBuildingNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		DetailBox->AddChildToVerticalBox(SelectedBuildingNameText);

		// Selected building cost
		SelectedBuildingCostText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedCost"));
		SelectedBuildingCostText->SetText(FText::GetEmpty());
		FSlateFontInfo CostFont = SelectedBuildingCostText->GetFont();
		CostFont.Size = 12;
		SelectedBuildingCostText->SetFont(CostFont);
		SelectedBuildingCostText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		DetailBox->AddChildToVerticalBox(SelectedBuildingCostText);

		// Selected building description
		SelectedBuildingDescText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedDesc"));
		SelectedBuildingDescText->SetText(FText::GetEmpty());
		FSlateFontInfo DescFont = SelectedBuildingDescText->GetFont();
		DescFont.Size = 10;
		SelectedBuildingDescText->SetFont(DescFont);
		SelectedBuildingDescText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
		DetailBox->AddChildToVerticalBox(SelectedBuildingDescText);
	}

	return Super::RebuildWidget();
}

void UBuildMenuWidget::ShowCategory(EBuildingCategory Category)
{
	CurrentCategory = Category;

	if (CategoryTitleText)
	{
		const UEnum* Enum = StaticEnum<EBuildingCategory>();
		if (Enum)
		{
			FText DisplayName = Enum->GetDisplayNameTextByValue(static_cast<int64>(Category));
			CategoryTitleText->SetText(DisplayName);
		}
	}

	RefreshBuildingList();

	UE_LOG(LogZooKeeper, Log, TEXT("BuildMenuWidget: Showing category '%s'."),
		*StaticEnum<EBuildingCategory>()->GetNameStringByValue(static_cast<int64>(Category)));
}

void UBuildMenuWidget::SelectBuilding(UBuildingDefinition* Definition)
{
	if (!Definition)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildMenuWidget: Attempted to select a null building definition."));
		return;
	}

	SelectedDefinition = Definition;

	if (SelectedBuildingNameText)
	{
		SelectedBuildingNameText->SetText(Definition->DisplayName);
	}

	if (SelectedBuildingCostText)
	{
		SelectedBuildingCostText->SetText(FText::FromString(FString::Printf(TEXT("$%d"), Definition->PurchaseCost)));
	}

	if (SelectedBuildingDescText)
	{
		SelectedBuildingDescText->SetText(Definition->Description);
	}

	OnBuildingSelected.Broadcast(Definition);

	UE_LOG(LogZooKeeper, Log, TEXT("BuildMenuWidget: Selected building '%s' (Cost: %d)."),
		*Definition->DisplayName.ToString(), Definition->PurchaseCost);
}

void UBuildMenuWidget::RefreshBuildingList()
{
	if (!BuildingListPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildMenuWidget: BuildingListPanel is not bound."));
		return;
	}

	BuildingListPanel->ClearChildren();

	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("BuildingDefinition"), AssetIds);

	int32 ItemCount = 0;
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		if (UBuildingDefinition* BuildingDef = Cast<UBuildingDefinition>(AssetPath.TryLoad()))
		{
			if (BuildingDef->Category == CurrentCategory)
			{
				// Create a text entry for each building in the list
				UTextBlock* EntryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
					*FString::Printf(TEXT("BuildEntry_%d"), ItemCount));

				const FString EntryStr = FString::Printf(TEXT("%s  -  $%d"),
					*BuildingDef->DisplayName.ToString(), BuildingDef->PurchaseCost);
				EntryText->SetText(FText::FromString(EntryStr));

				FSlateFontInfo EntryFont = EntryText->GetFont();
				EntryFont.Size = 12;
				EntryText->SetFont(EntryFont);
				EntryText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

				BuildingListPanel->AddChild(EntryText);
				ItemCount++;

				UE_LOG(LogZooKeeper, Verbose, TEXT("BuildMenuWidget: Added building '%s' to list."),
					*BuildingDef->DisplayName.ToString());
			}
		}
	}

	if (ItemCount == 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyListText"));
		EmptyText->SetText(FText::FromString(TEXT("No buildings available in this category.")));
		FSlateFontInfo EmptyFont = EmptyText->GetFont();
		EmptyFont.Size = 11;
		EmptyText->SetFont(EmptyFont);
		EmptyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		BuildingListPanel->AddChild(EmptyText);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("BuildMenuWidget: Refreshed list with %d buildings."), ItemCount);
}

void UBuildMenuWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
	RefreshBuildingList();

	UE_LOG(LogZooKeeper, Log, TEXT("BuildMenuWidget: Shown."));
}

void UBuildMenuWidget::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
	SelectedDefinition = nullptr;

	UE_LOG(LogZooKeeper, Log, TEXT("BuildMenuWidget: Hidden."));
}
