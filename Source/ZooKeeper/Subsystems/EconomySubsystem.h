#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Economy/EconomyTypes.h"
#include "EconomySubsystem.generated.h"

/** Broadcast when the zoo's fund balance changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEconomyFundsChanged, int32, NewBalance);

/** Broadcast when any transaction completes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransactionCompleted, const FZooTransaction&, Transaction);

/** Broadcast when funds reach zero or below. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBankruptcy);

/**
 * UEconomySubsystem
 *
 * World subsystem that manages the zoo's finances, including income, expenses,
 * transaction logging, and daily financial reports.
 */
UCLASS(meta = (DisplayName = "Economy Subsystem"))
class ZOOKEEPER_API UEconomySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Transactions
	// -------------------------------------------------------------------

	/**
	 * Attempts to spend the given amount. Fails if insufficient funds.
	 * @param Amount  The amount to spend (must be positive).
	 * @param Reason  A description of the expense.
	 * @return true if the funds were available and the expense was recorded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	bool TrySpend(int32 Amount, FString Reason);

	/**
	 * Adds income to the zoo's balance.
	 * @param Amount  The amount to add (must be positive).
	 * @param Reason  A description of the income source.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	void AddIncome(int32 Amount, FString Reason);

	/** Returns the current fund balance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Economy")
	int32 GetBalance() const;

	/**
	 * Processes recurring daily expenses such as staff salaries, animal feed,
	 * and maintenance costs. Should be called once per in-game day.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	void ProcessDailyExpenses();

	/**
	 * Returns a report summarizing today's financial activity.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Economy")
	FZooDailyFinanceReport GetDailyReport() const;

	// -------------------------------------------------------------------
	//  Loan System
	// -------------------------------------------------------------------

	/** Takes a loan of the given amount. Adds funds and increases loan balance. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	void TakeLoan(int32 Amount);

	/** Manually repays part or all of the current loan. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Economy")
	void RepayLoan(int32 Amount);

	/** Returns the current outstanding loan balance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Economy")
	int32 GetLoanBalance() const { return LoanBalance; }

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Economy")
	FOnEconomyFundsChanged OnFundsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Economy")
	FOnTransactionCompleted OnTransactionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Economy")
	FOnBankruptcy OnBankruptcy;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** The zoo's current available funds. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	int32 CurrentFunds;

private:
	/** Called when the day changes — triggers daily expense processing. */
	UFUNCTION()
	void HandleDayChanged(int32 NewDay);

	/** Log of all transactions for the current day. Cleared at the start of each new day. */
	UPROPERTY()
	TArray<FZooTransaction> TransactionLog;

	/** Accumulated daily expense total used for reporting. */
	UPROPERTY()
	TArray<FZooTransaction> DailyExpenseLog;

	/** Current outstanding loan balance. */
	UPROPERTY()
	int32 LoanBalance = 0;
};
