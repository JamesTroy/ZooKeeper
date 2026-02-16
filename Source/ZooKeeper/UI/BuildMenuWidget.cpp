#include "BuildMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Data/BuildingDefinition.h"
#include "Engine/AssetManager.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UBuildMenuWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UTextBlock* Placeholder = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Placeholder"));
		Placeholder->SetText(FText::FromString(TEXT("[Build Menu - Coming Soon]")));
		FSlateFontInfo Font = Placeholder->GetFont();
		Font.Size = 18;
		Placeholder->SetFont(Font);
		Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Placeholder);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);

		BuildingListPanel = nullptr;
		CategoryTitleText = nullptr;
		SelectedBuildingNameText = nullptr;
		SelectedBuildingCostText = nullptr;
		SelectedBuildingDescText = nullptr;
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

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		if (UBuildingDefinition* BuildingDef = Cast<UBuildingDefinition>(AssetPath.TryLoad()))
		{
			if (BuildingDef->Category == CurrentCategory)
			{
				UE_LOG(LogZooKeeper, Verbose, TEXT("BuildMenuWidget: Found building '%s' in category."),
					*BuildingDef->DisplayName.ToString());
			}
		}
	}
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
