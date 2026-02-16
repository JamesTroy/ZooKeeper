#include "FinancePanelWidget.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Subsystems/EconomySubsystem.h"
#include "ZooKeeper.h"

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

	// Update current funds display.
	if (CurrentFundsText)
	{
		CurrentFundsText->SetText(FText::FromString(FString::Printf(TEXT("$%d"), EconSys->GetBalance())));
	}

	// Retrieve the daily report for income/expenses.
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

	// Transaction entry widget creation is handled by the Blueprint child class.
	// The C++ base provides the data plumbing. Blueprint subclasses should override
	// this or bind to it to create the appropriate visual entry widget.
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

	// Clear existing transaction entries.
	if (TransactionListPanel)
	{
		TransactionListPanel->ClearChildren();
	}

	// Re-populate from the daily report.
	FZooDailyFinanceReport Report = EconSys->GetDailyReport();

	for (const FZooTransaction& Transaction : Report.IncomeTransactions)
	{
		AddTransactionEntry(Transaction);
	}

	for (const FZooTransaction& Transaction : Report.ExpenseTransactions)
	{
		AddTransactionEntry(Transaction);
	}

	// Update the summary fields.
	UpdateFinanceDisplay();

	UE_LOG(LogZooKeeper, Log, TEXT("FinancePanelWidget: Report refreshed with %d income and %d expense transactions."),
		Report.IncomeTransactions.Num(), Report.ExpenseTransactions.Num());
}
