#include "StaffRosterWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UStaffRosterWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// Main vertical layout
		UVerticalBox* MainLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
		UCanvasPanelSlot* MainSlot = RootCanvas->AddChildToCanvas(MainLayout);
		MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		MainSlot->SetOffsets(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

		// --- Title ---
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("STAFF ROSTER")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 20;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UVerticalBoxSlot* TitleSlot = MainLayout->AddChildToVerticalBox(TitleText);
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// --- Summary Bar ---
		UHorizontalBox* SummaryBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("SummaryBar"));
		UVerticalBoxSlot* SummarySlot = MainLayout->AddChildToVerticalBox(SummaryBar);
		SummarySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// Staff count label
		UTextBlock* StaffLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaffLabel"));
		StaffLabel->SetText(FText::FromString(TEXT("Staff: ")));
		FSlateFontInfo LabelFont = StaffLabel->GetFont();
		LabelFont.Size = 14;
		StaffLabel->SetFont(LabelFont);
		StaffLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		UHorizontalBoxSlot* StaffLabelSlot = SummaryBar->AddChildToHorizontalBox(StaffLabel);
		StaffLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 4.0f, 0.0f));

		// Staff count value
		TotalStaffText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TotalStaff"));
		TotalStaffText->SetText(FText::FromString(TEXT("0")));
		TotalStaffText->SetFont(LabelFont);
		TotalStaffText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UHorizontalBoxSlot* StaffValSlot = SummaryBar->AddChildToHorizontalBox(TotalStaffText);
		StaffValSlot->SetPadding(FMargin(0.0f, 0.0f, 20.0f, 0.0f));

		// Salary label
		UTextBlock* SalaryLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SalaryLabel"));
		SalaryLabel->SetText(FText::FromString(TEXT("Daily Salaries: ")));
		SalaryLabel->SetFont(LabelFont);
		SalaryLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		SummaryBar->AddChildToHorizontalBox(SalaryLabel);

		// Salary value
		TotalSalaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TotalSalary"));
		TotalSalaryText->SetText(FText::FromString(TEXT("$0/day")));
		TotalSalaryText->SetFont(LabelFont);
		TotalSalaryText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.4f, 0.4f)));
		SummaryBar->AddChildToHorizontalBox(TotalSalaryText);

		// --- Hire Buttons ---
		UHorizontalBox* HireBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HireBar"));
		UVerticalBoxSlot* HireSlot = MainLayout->AddChildToVerticalBox(HireBar);
		HireSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		const TArray<FString> HireLabels = {
			TEXT("[Hire Zookeeper]"), TEXT("[Hire Vet]"), TEXT("[Hire Janitor]"),
			TEXT("[Hire Mechanic]"), TEXT("[Hire Guide]")
		};

		for (int32 i = 0; i < HireLabels.Num(); ++i)
		{
			UTextBlock* HireBtn = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
				*FString::Printf(TEXT("HireBtn_%d"), i));
			HireBtn->SetText(FText::FromString(HireLabels[i]));
			FSlateFontInfo BtnFont = HireBtn->GetFont();
			BtnFont.Size = 11;
			HireBtn->SetFont(BtnFont);
			HireBtn->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
			UHorizontalBoxSlot* BtnSlot = HireBar->AddChildToHorizontalBox(HireBtn);
			BtnSlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));
		}

		// --- Staff List (ScrollBox) ---
		UScrollBox* StaffScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("StaffScroll"));
		UVerticalBoxSlot* ScrollSlot = MainLayout->AddChildToVerticalBox(StaffScroll);
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		StaffListPanel = StaffScroll;
	}

	return Super::RebuildWidget();
}

void UStaffRosterWidget::RefreshStaffList()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UStaffSubsystem* StaffSub = World->GetSubsystem<UStaffSubsystem>();
	if (!StaffSub)
	{
		return;
	}

	// Update summary
	if (TotalStaffText)
	{
		TotalStaffText->SetText(FText::FromString(FString::Printf(TEXT("%d"), StaffSub->GetStaffCount())));
	}

	if (TotalSalaryText)
	{
		TotalSalaryText->SetText(FText::FromString(FString::Printf(TEXT("$%d/day"), StaffSub->GetDailySalaryCost())));
	}

	// Update staff list
	if (!StaffListPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffRosterWidget: StaffListPanel is not bound."));
		return;
	}

	StaffListPanel->ClearChildren();

	const TArray<FStaffRecord> Records = StaffSub->GetAllStaffRecords();
	if (Records.Num() == 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyListText"));
		EmptyText->SetText(FText::FromString(TEXT("No staff hired yet.")));
		FSlateFontInfo EmptyFont = EmptyText->GetFont();
		EmptyFont.Size = 11;
		EmptyText->SetFont(EmptyFont);
		EmptyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		StaffListPanel->AddChild(EmptyText);
		return;
	}

	const UEnum* TypeEnum = StaticEnum<EStaffType>();
	int32 EntryIndex = 0;
	for (const FStaffRecord& Record : Records)
	{
		FString TypeStr = TypeEnum ? TypeEnum->GetDisplayNameTextByValue(static_cast<int64>(Record.Type)).ToString() : TEXT("Unknown");
		FString Assignment = Record.AssignedEnclosure ? Record.AssignedEnclosure->GetName() : TEXT("Unassigned");

		FString EntryStr = FString::Printf(TEXT("[ID:%d] %s  |  %s  |  $%d/day  |  Skill: %.0f%%  |  %s  [Fire]"),
			Record.StaffID, *Record.Name, *TypeStr, Record.Salary,
			Record.Skill * 100.0f, *Assignment);

		UTextBlock* EntryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
			*FString::Printf(TEXT("StaffEntry_%d"), EntryIndex++));
		EntryText->SetText(FText::FromString(EntryStr));
		FSlateFontInfo EntryFont = EntryText->GetFont();
		EntryFont.Size = 11;
		EntryText->SetFont(EntryFont);
		EntryText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		StaffListPanel->AddChild(EntryText);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Refreshed with %d staff entries."), Records.Num());
}

void UStaffRosterWidget::HireStaffClicked(EStaffType Type)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UStaffSubsystem* StaffSub = World->GetSubsystem<UStaffSubsystem>();
	if (!StaffSub)
	{
		return;
	}

	// Generate a name based on type
	const UEnum* TypeEnum = StaticEnum<EStaffType>();
	FString TypeName = TypeEnum ? TypeEnum->GetDisplayNameTextByValue(static_cast<int64>(Type)).ToString() : TEXT("Staff");
	FString NewName = FString::Printf(TEXT("%s %d"), *TypeName, StaffSub->GetStaffCount() + 1);

	const int32 NewID = StaffSub->HireStaff(Type, NewName);
	if (NewID != INDEX_NONE)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Hired '%s' (ID: %d)."), *NewName, NewID);
	}

	RefreshStaffList();
}

void UStaffRosterWidget::FireStaffClicked(int32 StaffID)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UStaffSubsystem* StaffSub = World->GetSubsystem<UStaffSubsystem>();
	if (StaffSub)
	{
		StaffSub->FireStaff(StaffID);
	}

	RefreshStaffList();
}
