#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinancePanelWidget.generated.h"

class UTextBlock;
class UPanelWidget;
struct FZooTransaction;

/**
 * UFinancePanelWidget
 *
 * Widget that displays the zoo's financial information, including current
 * funds, daily income, daily expenses, net profit, and a scrollable list
 * of recent transactions.
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_FinancePanel).
 */
UCLASS(meta = (DisplayName = "Finance Panel Widget"))
class ZOOKEEPER_API UFinancePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Displays the zoo's current fund balance. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Finance")
	UTextBlock* CurrentFundsText;

	/** Displays the total daily income. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Finance")
	UTextBlock* DailyIncomeText;

	/** Displays the total daily expenses. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Finance")
	UTextBlock* DailyExpensesText;

	/** Displays the net profit (income minus expenses). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Finance")
	UTextBlock* NetProfitText;

	/** Scrollable panel containing individual transaction entries. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Finance")
	UPanelWidget* TransactionListPanel;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/** Reads from the EconomySubsystem and updates all financial display fields. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void UpdateFinanceDisplay();

	/**
	 * Adds a single transaction entry to the transaction list panel.
	 * @param Transaction  The transaction data to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void AddTransactionEntry(const FZooTransaction& Transaction);

	/** Clears and rebuilds the full transaction list from the EconomySubsystem. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void RefreshReport();
};
