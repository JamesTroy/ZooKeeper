#include "EconomySubsystem.h"
#include "ZooKeeper.h"

bool UEconomySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentFunds = 50000;

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem::Initialize - Starting funds: %d"), CurrentFunds);
}

void UEconomySubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem::Deinitialize - Final balance: %d, Total transactions: %d"),
		CurrentFunds, TransactionLog.Num());

	TransactionLog.Empty();
	DailyExpenseLog.Empty();

	Super::Deinitialize();
}

bool UEconomySubsystem::TrySpend(int32 Amount, FString Reason)
{
	if (Amount <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("EconomySubsystem::TrySpend - Invalid amount: %d"), Amount);
		return false;
	}

	if (CurrentFunds < Amount)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("EconomySubsystem::TrySpend - Insufficient funds. Have: %d, Need: %d, Reason: %s"),
			CurrentFunds, Amount, *Reason);
		return false;
	}

	CurrentFunds -= Amount;

	FZooTransaction Transaction;
	Transaction.Amount = Amount;
	Transaction.Reason = MoveTemp(Reason);
	Transaction.bIsExpense = true;
	Transaction.GameTime = 0.0f; // Will be set by caller or time subsystem integration

	TransactionLog.Add(Transaction);
	OnTransactionCompleted.Broadcast(Transaction);
	OnFundsChanged.Broadcast(CurrentFunds);

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem - Spent %d for '%s'. Balance: %d"),
		Transaction.Amount, *Transaction.Reason, CurrentFunds);

	if (CurrentFunds <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("EconomySubsystem - BANKRUPTCY! Funds have reached zero."));
		OnBankruptcy.Broadcast();
	}

	return true;
}

void UEconomySubsystem::AddIncome(int32 Amount, FString Reason)
{
	if (Amount <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("EconomySubsystem::AddIncome - Invalid amount: %d"), Amount);
		return;
	}

	CurrentFunds += Amount;

	FZooTransaction Transaction;
	Transaction.Amount = Amount;
	Transaction.Reason = MoveTemp(Reason);
	Transaction.bIsExpense = false;
	Transaction.GameTime = 0.0f;

	TransactionLog.Add(Transaction);
	OnTransactionCompleted.Broadcast(Transaction);
	OnFundsChanged.Broadcast(CurrentFunds);

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem - Income of %d from '%s'. Balance: %d"),
		Transaction.Amount, *Transaction.Reason, CurrentFunds);
}

int32 UEconomySubsystem::GetBalance() const
{
	return CurrentFunds;
}

void UEconomySubsystem::ProcessDailyExpenses()
{
	// Clear the daily expense log from the previous day
	DailyExpenseLog.Empty();

	// Copy today's expense transactions into the daily log before clearing
	for (const FZooTransaction& Transaction : TransactionLog)
	{
		if (Transaction.bIsExpense)
		{
			DailyExpenseLog.Add(Transaction);
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem::ProcessDailyExpenses - Processed %d transactions. Balance: %d"),
		TransactionLog.Num(), CurrentFunds);

	// Clear the transaction log for the new day
	TransactionLog.Empty();
}

FZooDailyFinanceReport UEconomySubsystem::GetDailyReport() const
{
	FZooDailyFinanceReport Report;

	for (const FZooTransaction& Transaction : TransactionLog)
	{
		if (Transaction.bIsExpense)
		{
			Report.TotalExpenses += Transaction.Amount;
			Report.ExpenseTransactions.Add(Transaction);
		}
		else
		{
			Report.TotalIncome += Transaction.Amount;
			Report.IncomeTransactions.Add(Transaction);
		}
	}

	return Report;
}
