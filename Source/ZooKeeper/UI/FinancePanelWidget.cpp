#include "FinancePanelWidget.h"

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
#include "Subsystems/EconomySubsystem.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UFinancePanelWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		// Main layout
		UVerticalBox* MainLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
		UCanvasPanelSlot* MainSlot = RootCanvas->AddChildToCanvas(MainLayout);
		MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		MainSlot->SetOffsets(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

		FSlateFontInfo LabelFont;
		{
			UTextBlock* Temp = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FontHelper"));
			LabelFont = Temp->GetFont();
			LabelFont.Size = 14;
		}

		// --- Title ---
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("FINANCES")));
		FSlateFontInfo TitleFont = LabelFont;
		TitleFont.Size = 20;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UVerticalBoxSlot* TitleSlot = MainLayout->AddChildToVerticalBox(TitleText);
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		// --- Current Funds ---
		UHorizontalBox* FundsRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("FundsRow"));
		MainLayout->AddChildToVerticalBox(FundsRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		UTextBlock* FundsLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FundsLabel"));
		FundsLabel->SetText(FText::FromString(TEXT("Current Funds: ")));
		FundsLabel->SetFont(LabelFont);
		FundsLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		FundsRow->AddChildToHorizontalBox(FundsLabel);

		CurrentFundsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CurrentFunds"));
		CurrentFundsText->SetText(FText::FromString(TEXT("$0")));
		FSlateFontInfo FundsValFont = LabelFont;
		FundsValFont.Size = 16;
		CurrentFundsText->SetFont(FundsValFont);
		CurrentFundsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.9f, 0.2f)));
		FundsRow->AddChildToHorizontalBox(CurrentFundsText);

		// --- Daily Income ---
		UHorizontalBox* IncomeRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("IncomeRow"));
		MainLayout->AddChildToVerticalBox(IncomeRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		UTextBlock* IncomeLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("IncomeLabel"));
		IncomeLabel->SetText(FText::FromString(TEXT("Daily Income: ")));
		IncomeLabel->SetFont(LabelFont);
		IncomeLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		IncomeRow->AddChildToHorizontalBox(IncomeLabel);

		DailyIncomeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DailyIncome"));
		DailyIncomeText->SetText(FText::FromString(TEXT("+$0")));
		DailyIncomeText->SetFont(LabelFont);
		DailyIncomeText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
		IncomeRow->AddChildToHorizontalBox(DailyIncomeText);

		// --- Daily Expenses ---
		UHorizontalBox* ExpenseRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ExpenseRow"));
		MainLayout->AddChildToVerticalBox(ExpenseRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		UTextBlock* ExpenseLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ExpenseLabel"));
		ExpenseLabel->SetText(FText::FromString(TEXT("Daily Expenses: ")));
		ExpenseLabel->SetFont(LabelFont);
		ExpenseLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		ExpenseRow->AddChildToHorizontalBox(ExpenseLabel);

		DailyExpensesText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DailyExpenses"));
		DailyExpensesText->SetText(FText::FromString(TEXT("-$0")));
		DailyExpensesText->SetFont(LabelFont);
		DailyExpensesText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.4f, 0.4f)));
		ExpenseRow->AddChildToHorizontalBox(DailyExpensesText);

		// --- Net Profit ---
		UHorizontalBox* NetRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("NetRow"));
		MainLayout->AddChildToVerticalBox(NetRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

		UTextBlock* NetLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NetLabel"));
		NetLabel->SetText(FText::FromString(TEXT("Net Profit: ")));
		NetLabel->SetFont(LabelFont);
		NetLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		NetRow->AddChildToHorizontalBox(NetLabel);

		NetProfitText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NetProfit"));
		NetProfitText->SetText(FText::FromString(TEXT("$0")));
		FSlateFontInfo NetFont = LabelFont;
		NetFont.Size = 16;
		NetProfitText->SetFont(NetFont);
		NetProfitText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		NetRow->AddChildToHorizontalBox(NetProfitText);

		// --- Transaction History Header ---
		UTextBlock* TxnHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TxnHeader"));
		TxnHeader->SetText(FText::FromString(TEXT("Recent Transactions:")));
		TxnHeader->SetFont(LabelFont);
		TxnHeader->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
		MainLayout->AddChildToVerticalBox(TxnHeader)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));

		// --- Transaction List ---
		UScrollBox* TxnScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("TxnScroll"));
		UVerticalBoxSlot* ScrollSlot = MainLayout->AddChildToVerticalBox(TxnScroll);
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		TransactionListPanel = TxnScroll;
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

	static int32 TxnIndex = 0;
	FString Sign = Transaction.bIsExpense ? TEXT("-") : TEXT("+");
	FString TxnStr = FString::Printf(TEXT("  %s$%d  |  %s"),
		*Sign, Transaction.Amount, *Transaction.Reason);

	UTextBlock* TxnText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("Txn_%d"), TxnIndex++));
	TxnText->SetText(FText::FromString(TxnStr));
	FSlateFontInfo TxnFont = TxnText->GetFont();
	TxnFont.Size = 11;
	TxnText->SetFont(TxnFont);
	TxnText->SetColorAndOpacity(FSlateColor(
		Transaction.bIsExpense ? FLinearColor(1.0f, 0.5f, 0.5f) : FLinearColor(0.5f, 1.0f, 0.5f)));
	TransactionListPanel->AddChild(TxnText);
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
