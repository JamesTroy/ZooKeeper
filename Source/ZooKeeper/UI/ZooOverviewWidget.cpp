#include "ZooOverviewWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Core/ZooGameState.h"
#include "Subsystems/AnimalManagerSubsystem.h"
#include "Subsystems/VisitorSubsystem.h"
#include "Subsystems/StaffSubsystem.h"
#include "Subsystems/ZooRatingSubsystem.h"
#include "ZooKeeper.h"

static UTextBlock* CreateOverviewRow(UWidgetTree* Tree, UHorizontalBox* Parent, const FString& Label, const FString& WidgetName)
{
	FSlateFontInfo LabelFont;
	{
		UTextBlock* Temp = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TempFont"));
		LabelFont = Temp->GetFont();
		LabelFont.Size = 14;
	}

	UTextBlock* LabelText = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("%s_Label"), *WidgetName));
	LabelText->SetText(FText::FromString(Label));
	LabelText->SetFont(LabelFont);
	LabelText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	UHorizontalBoxSlot* LSlot = Parent->AddChildToHorizontalBox(LabelText);
	LSlot->SetPadding(FMargin(0.0f, 0.0f, 4.0f, 0.0f));

	UTextBlock* ValueText = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("%s_Value"), *WidgetName));
	ValueText->SetText(FText::FromString(TEXT("--")));
	ValueText->SetFont(LabelFont);
	ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Parent->AddChildToHorizontalBox(ValueText);

	return ValueText;
}

TSharedRef<SWidget> UZooOverviewWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UVerticalBox* MainLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainLayout"));
		UCanvasPanelSlot* MainSlot = RootCanvas->AddChildToCanvas(MainLayout);
		MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		MainSlot->SetOffsets(FMargin(10.0f, 10.0f, 10.0f, 10.0f));

		// --- Title ---
		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
		TitleText->SetText(FText::FromString(TEXT("ZOO OVERVIEW")));
		FSlateFontInfo TitleFont = TitleText->GetFont();
		TitleFont.Size = 20;
		TitleText->SetFont(TitleFont);
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		MainLayout->AddChildToVerticalBox(TitleText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

		// --- Zoo Name Row ---
		UHorizontalBox* NameRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("NameRow"));
		MainLayout->AddChildToVerticalBox(NameRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		ZooNameText = CreateOverviewRow(WidgetTree, NameRow, TEXT("Zoo: "), TEXT("ZooName"));

		// --- Rating Row ---
		UHorizontalBox* RatingRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("RatingRow"));
		MainLayout->AddChildToVerticalBox(RatingRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		ReputationText = CreateOverviewRow(WidgetTree, RatingRow, TEXT("Rating: "), TEXT("Rating"));

		// --- Animal Count Row ---
		UHorizontalBox* AnimalRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("AnimalRow"));
		MainLayout->AddChildToVerticalBox(AnimalRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		AnimalCountText = CreateOverviewRow(WidgetTree, AnimalRow, TEXT("Animals: "), TEXT("Animals"));

		// --- Visitor Count Row ---
		UHorizontalBox* VisitorRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("VisitorRow"));
		MainLayout->AddChildToVerticalBox(VisitorRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		VisitorCountText = CreateOverviewRow(WidgetTree, VisitorRow, TEXT("Visitors: "), TEXT("Visitors"));

		// --- Staff Count Row ---
		UHorizontalBox* StaffRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("StaffRow"));
		MainLayout->AddChildToVerticalBox(StaffRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		StaffCountText = CreateOverviewRow(WidgetTree, StaffRow, TEXT("Staff: "), TEXT("Staff"));

		// --- Weather Row ---
		UHorizontalBox* WeatherRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("WeatherRow"));
		MainLayout->AddChildToVerticalBox(WeatherRow)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		WeatherText = CreateOverviewRow(WidgetTree, WeatherRow, TEXT("Weather: "), TEXT("Weather"));
	}

	return Super::RebuildWidget();
}

void UZooOverviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshTimer = 0.0f;
	RefreshOverview();
}

void UZooOverviewWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshTimer += InDeltaTime;
	if (RefreshTimer >= RefreshInterval)
	{
		RefreshTimer = 0.0f;
		RefreshOverview();
	}
}

void UZooOverviewWidget::RefreshOverview()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Zoo Name
	if (ZooNameText)
	{
		ZooNameText->SetText(FText::FromString(TEXT("My Zoo")));
	}

	// Zoo Rating from ZooRatingSubsystem
	if (ReputationText)
	{
		if (UZooRatingSubsystem* RatingSub = World->GetSubsystem<UZooRatingSubsystem>())
		{
			float Rating = RatingSub->GetRating();
			// Build star string
			int32 FullStars = FMath::FloorToInt(Rating);
			bool bHalfStar = (Rating - FullStars) >= 0.5f;
			FString Stars;
			for (int32 i = 0; i < FullStars; ++i) Stars += TEXT("*");
			if (bHalfStar) Stars += TEXT("~");
			ReputationText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / 5.0  %s"), Rating, *Stars)));
		}
		else
		{
			AZooGameState* GameState = Cast<AZooGameState>(World->GetGameState());
			if (GameState)
			{
				ReputationText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / 5.0"), GameState->GetReputation())));
			}
		}
	}

	// Animal Count
	if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
	{
		if (AnimalCountText)
		{
			AnimalCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), AnimalMgr->GetAnimalCount())));
		}
	}

	// Visitor Count
	if (UVisitorSubsystem* VisitorSub = World->GetSubsystem<UVisitorSubsystem>())
	{
		if (VisitorCountText)
		{
			VisitorCountText->SetText(FText::FromString(
				FString::Printf(TEXT("%d / %d"), VisitorSub->CurrentVisitorCount, VisitorSub->MaxVisitors)));
		}
	}

	// Staff Count
	if (UStaffSubsystem* StaffSub = World->GetSubsystem<UStaffSubsystem>())
	{
		if (StaffCountText)
		{
			StaffCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), StaffSub->GetStaffCount())));
		}
	}

	// Weather
	if (WeatherText)
	{
		AZooGameState* GameState = Cast<AZooGameState>(World->GetGameState());
		if (GameState)
		{
			FName Weather = GameState->GetWeather();
			WeatherText->SetText(Weather.IsNone()
				? FText::FromString(TEXT("Clear"))
				: FText::FromName(Weather));
		}
	}
}
