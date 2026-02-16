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
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Finance Panel Widget"))
class ZOOKEEPER_API UFinancePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void UpdateFinanceDisplay();

	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void AddTransactionEntry(const FZooTransaction& Transaction);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Finance")
	void RefreshReport();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TObjectPtr<UTextBlock> CurrentFundsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> DailyIncomeText;

	UPROPERTY()
	TObjectPtr<UTextBlock> DailyExpensesText;

	UPROPERTY()
	TObjectPtr<UTextBlock> NetProfitText;

	UPROPERTY()
	TObjectPtr<UPanelWidget> TransactionListPanel;
};
