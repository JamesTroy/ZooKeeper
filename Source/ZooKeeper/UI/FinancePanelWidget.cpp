#include "FinancePanelWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystems/EconomySubsystem.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UFinancePanelWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UTextBlock* Placeholder = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Placeholder"));
		Placeholder->SetText(FText::FromString(TEXT("[Finance Panel - Coming Soon]")));
		FSlateFontInfo Font = Placeholder->GetFont();
		Font.Size = 18;
		Placeholder->SetFont(Font);
		Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Placeholder);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);

		CurrentFundsText = nullptr;
		DailyIncomeText = nullptr;
		DailyExpensesText = nullptr;
		NetProfitText = nullptr;
		TransactionListPanel = nullptr;
	}

	return Super::RebuildWidget();
}

void UFinancePanelWidget::UpdateFinanceDisplay()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("FinancePanelWidget: No world available."));
		return;
	}

	UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>();
	if (!EconSys)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("FinancePanelWidget: EconomySubsystem not found."));
		return;
	}

	if (CurrentFundsText)
	{
		CurrentFundsText->SetText(FText::FromString(FString::Printf(TEXT("$%d"), EconSys->GetBalance())));
	}

	FZooDailyFinanceReport Report = EconSys->GetDailyReport();

	if (DailyIncomeText)
	{
		DailyIncomeText->SetText(FText::FromString(FString::Printf(TEXT("+$%d"), Report.TotalIncome)));
	}

	if (DailyExpensesText)
	{
		DailyExpensesText->SetText(FText::FromString(FString::Printf(TEXT("-$%d"), Report.TotalExpenses)));
	}

	int32 NetProfit = Report.TotalIncome - Report.TotalExpenses;
	if (NetProfitText)
	{
		FString ProfitStr = NetProfit >= 0
			? FString::Printf(TEXT("+$%d"), NetProfit)
			: FString::Printf(TEXT("-$%d"), FMath::Abs(NetProfit));
		NetProfitText->SetText(FText::FromString(ProfitStr));
	}

	UE_LOG(LogZooKeeper, Verbose, TEXT("FinancePanelWidget: Updated. Funds=%d Income=%d Expenses=%d Net=%d"),
		EconSys->GetBalance(), Report.TotalIncome, Report.TotalExpenses, NetProfit);
}

void UFinancePanelWidget::AddTransactionEntry(const FZooTransaction& Transaction)
{
	if (!TransactionListPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("FinancePanelWidget: TransactionListPanel is not bound."));
		return;
	}

	UE_LOG(LogZooKeeper, Verbose, TEXT("FinancePanelWidget: Transaction entry - %s: %s$%d"),
		Transaction.bIsExpense ? TEXT("Expense") : TEXT("Income"),
		Transaction.bIsExpense ? TEXT("-") : TEXT("+"),
		Transaction.Amount);
}

void UFinancePanelWidget::RefreshReport()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>();
	if (!EconSys)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("FinancePanelWidget: EconomySubsystem not found during RefreshReport."));
		return;
	}

	if (TransactionListPanel)
	{
		TransactionListPanel->ClearChildren();
	}

	FZooDailyFinanceReport Report = EconSys->GetDailyReport();

	for (const FZooTransaction& Transaction : Report.Transactions)
	{
		AddTransactionEntry(Transaction);
	}

	UpdateFinanceDisplay();

	UE_LOG(LogZooKeeper, Log, TEXT("FinancePanelWidget: Report refreshed with %d transactions."),
		Report.Transactions.Num());
}
