#include "EconomySubsystem.h"
#include "StaffSubsystem.h"
#include "AnimalManagerSubsystem.h"
#include "TimeSubsystem.h"
#include "ZooKeeper.h"

bool UEconomySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentFunds = 50000;

	// Subscribe to day changes to trigger daily expense processing.
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->OnDayChanged.AddDynamic(this, &UEconomySubsystem::HandleDayChanged);
		}
	}

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
	// Set time from TimeSubsystem if available.
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			Transaction.GameTime = TimeSys->CurrentTimeOfDay;
			Transaction.Day = TimeSys->CurrentDay;
		}
	}

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

	// Set time from TimeSubsystem if available.
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			Transaction.GameTime = TimeSys->CurrentTimeOfDay;
			Transaction.Day = TimeSys->CurrentDay;
		}
	}

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
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Archive yesterday's transactions into the daily expense log.
	DailyExpenseLog = TransactionLog;
	TransactionLog.Empty();

	// --- Staff Salaries ---
	if (UStaffSubsystem* StaffSys = World->GetSubsystem<UStaffSubsystem>())
	{
		const int32 SalaryCost = StaffSys->GetDailySalaryCost();
		if (SalaryCost > 0)
		{
			TrySpend(SalaryCost, TEXT("Staff salaries"));
		}
	}

	// --- Animal Food Costs ---
	// Estimate daily food cost as $5 per animal.
	if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
	{
		const int32 AnimalCount = AnimalMgr->GetAnimalCount();
		const int32 FoodCost = AnimalCount * 5;
		if (FoodCost > 0)
		{
			TrySpend(FoodCost, FString::Printf(TEXT("Animal food (%d animals)"), AnimalCount));
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem::ProcessDailyExpenses - Daily expenses processed. Balance: %d"),
		CurrentFunds);
}

FZooDailyFinanceReport UEconomySubsystem::GetDailyReport() const
{
	FZooDailyFinanceReport Report;
	Report.Transactions = TransactionLog;

	for (const FZooTransaction& Transaction : TransactionLog)
	{
		if (Transaction.bIsExpense)
		{
			Report.TotalExpenses += Transaction.Amount;
		}
		else
		{
			Report.TotalIncome += Transaction.Amount;
		}
	}

	Report.NetProfit = Report.TotalIncome - Report.TotalExpenses;

	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			Report.Day = TimeSys->CurrentDay;
		}
	}

	return Report;
}

void UEconomySubsystem::TakeLoan(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	LoanBalance += Amount;
	AddIncome(Amount, FString::Printf(TEXT("Loan taken ($%d)"), Amount));

	UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem - Loan taken: $%d. Total debt: $%d"), Amount, LoanBalance);
}

void UEconomySubsystem::RepayLoan(int32 Amount)
{
	if (Amount <= 0 || LoanBalance <= 0)
	{
		return;
	}

	const int32 RepayAmount = FMath::Min(Amount, LoanBalance);
	if (TrySpend(RepayAmount, FString::Printf(TEXT("Loan repayment ($%d)"), RepayAmount)))
	{
		LoanBalance -= RepayAmount;
		UE_LOG(LogZooKeeper, Log, TEXT("EconomySubsystem - Loan repaid: $%d. Remaining debt: $%d"), RepayAmount, LoanBalance);
	}
}

void UEconomySubsystem::HandleDayChanged(int32 NewDay)
{
	ProcessDailyExpenses();

	// Auto-repay a portion of the loan each day (10% of balance or $500, whichever is greater)
	if (LoanBalance > 0)
	{
		const int32 AutoRepay = FMath::Max(LoanBalance / 10, FMath::Min(500, LoanBalance));
		RepayLoan(AutoRepay);
	}
}
