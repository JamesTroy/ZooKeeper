#pragma once

#include "CoreMinimal.h"
#include "EconomyTypes.generated.h"

/**
 * ETransactionCategory
 *
 * Categorizes financial transactions for reporting and filtering.
 */
UENUM(BlueprintType)
enum class ETransactionCategory : uint8
{
	AnimalPurchase			UMETA(DisplayName = "Animal Purchase"),
	AnimalFood				UMETA(DisplayName = "Animal Food"),
	BuildingPurchase		UMETA(DisplayName = "Building Purchase"),
	BuildingMaintenance		UMETA(DisplayName = "Building Maintenance"),
	StaffSalary				UMETA(DisplayName = "Staff Salary"),
	VisitorTicket			UMETA(DisplayName = "Visitor Ticket"),
	VisitorFood				UMETA(DisplayName = "Visitor Food"),
	Research				UMETA(DisplayName = "Research"),
	Miscellaneous			UMETA(DisplayName = "Miscellaneous")
};

/**
 * FZooTransaction
 *
 * Represents a single financial transaction within the zoo.
 * Tracks the amount, reason, direction (income vs. expense),
 * and when the transaction occurred.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooTransaction
{
	GENERATED_BODY()

	/** The monetary amount of this transaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 Amount = 0;

	/** Human-readable description of why the transaction occurred. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	FString Reason;

	/** True if this transaction is an expense; false if it is income. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	bool bIsExpense = true;

	/** The in-game time (hours since midnight) when the transaction occurred. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	float GameTime = 0.0f;

	/** The in-game day on which the transaction occurred. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 Day = 0;

	/** The category of this transaction for reporting purposes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	ETransactionCategory Category = ETransactionCategory::Miscellaneous;
};

/**
 * FZooDailyFinanceReport
 *
 * Aggregated financial summary for a single in-game day.
 * Contains totals and the list of individual transactions that occurred.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooDailyFinanceReport
{
	GENERATED_BODY()

	/** The in-game day this report covers. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 Day = 0;

	/** Total income received during this day. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 TotalIncome = 0;

	/** Total expenses incurred during this day. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 TotalExpenses = 0;

	/** Net profit for this day (TotalIncome - TotalExpenses). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	int32 NetProfit = 0;

	/** All individual transactions that occurred during this day. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Economy")
	TArray<FZooTransaction> Transactions;
};
