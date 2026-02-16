#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EconomySubsystem.generated.h"

/**
 * FZooTransaction
 *
 * Record of a single financial transaction within the zoo.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooTransaction
{
	GENERATED_BODY()

	/** The monetary amount of this transaction (always positive). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	int32 Amount = 0;

	/** Human-readable reason for this transaction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	FString Reason;

	/** True if this transaction represents money spent; false for income. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	bool bIsExpense = false;

	/** The in-game time at which this transaction occurred. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	float GameTime = 0.0f;
};

/**
 * FZooDailyFinanceReport
 *
 * Summary of a single day's financial activity.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooDailyFinanceReport
{
	GENERATED_BODY()

	/** Total income earned during the day. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	int32 TotalIncome = 0;

	/** Total expenses paid during the day. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	int32 TotalExpenses = 0;

	/** All income transactions for the day. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	TArray<FZooTransaction> IncomeTransactions;

	/** All expense transactions for the day. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Economy")
	TArray<FZooTransaction> ExpenseTransactions;
};

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
	/** Log of all transactions for the current day. Cleared at the start of each new day. */
	UPROPERTY()
	TArray<FZooTransaction> TransactionLog;

	/** Accumulated daily expense total used for reporting. */
	UPROPERTY()
	TArray<FZooTransaction> DailyExpenseLog;
};
