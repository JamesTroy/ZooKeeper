#include "StaffRosterWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UStaffRosterWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UTextBlock* Placeholder = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Placeholder"));
		Placeholder->SetText(FText::FromString(TEXT("[Staff Roster - Coming Soon]")));
		FSlateFontInfo Font = Placeholder->GetFont();
		Font.Size = 18;
		Placeholder->SetFont(Font);
		Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Placeholder);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);

		TotalStaffText = nullptr;
		TotalSalaryText = nullptr;
		StaffListPanel = nullptr;
	}

	return Super::RebuildWidget();
}

void UStaffRosterWidget::RefreshStaffList()
{
	if (!StaffListPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffRosterWidget: StaffListPanel is not bound."));
		return;
	}

	StaffListPanel->ClearChildren();

	if (TotalStaffText)
	{
		TotalStaffText->SetText(FText::FromString(TEXT("0")));
	}

	if (TotalSalaryText)
	{
		TotalSalaryText->SetText(FText::FromString(TEXT("$0/day")));
	}

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Staff list refreshed."));
}

void UStaffRosterWidget::HireStaffClicked(EStaffType Type)
{
	const UEnum* StaffTypeEnum = StaticEnum<EStaffType>();
	FString TypeName = StaffTypeEnum ? StaffTypeEnum->GetNameStringByValue(static_cast<int64>(Type)) : TEXT("Unknown");

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Hire button clicked for staff type '%s'."), *TypeName);

	RefreshStaffList();
}

void UStaffRosterWidget::FireStaffClicked(int32 StaffID)
{
	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Fire button clicked for staff ID %d."), StaffID);

	RefreshStaffList();
}
