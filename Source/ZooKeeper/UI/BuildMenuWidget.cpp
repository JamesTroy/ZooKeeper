#include "BuildMenuWidget.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Data/BuildingDefinition.h"
#include "Engine/AssetManager.h"
#include "ZooKeeper.h"

void UBuildMenuWidget::ShowCategory(EBuildingCategory Category)
{
	CurrentCategory = Category;

	// Update the category title text.
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

	// Update detail display.
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

	// Broadcast the selection delegate.
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

	// Clear existing entries.
	BuildingListPanel->ClearChildren();

	// Load all building definitions via the Asset Manager and filter by current category.
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
				// Building entries will be created and added by the Blueprint child class.
				// The C++ base provides the filtering logic; Blueprint handles the visual
				// entry widget instantiation. Subclasses should override RefreshBuildingList
				// or use a BlueprintImplementableEvent for entry creation.
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
