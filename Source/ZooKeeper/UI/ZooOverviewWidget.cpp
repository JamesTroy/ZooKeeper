#include "ZooOverviewWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Core/ZooGameState.h"
#include "Subsystems/AnimalManagerSubsystem.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UZooOverviewWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UTextBlock* Placeholder = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Placeholder"));
		Placeholder->SetText(FText::FromString(TEXT("[Zoo Overview - Coming Soon]")));
		FSlateFontInfo Font = Placeholder->GetFont();
		Font.Size = 18;
		Placeholder->SetFont(Font);
		Placeholder->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Placeholder);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);

		ZooNameText = nullptr;
		ReputationText = nullptr;
		VisitorCountText = nullptr;
		AnimalCountText = nullptr;
		StaffCountText = nullptr;
		WeatherText = nullptr;
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

	AZooGameState* GameState = Cast<AZooGameState>(World->GetGameState());
	if (GameState)
	{
		if (ZooNameText)
		{
			ZooNameText->SetText(FText::FromString(TEXT("My Zoo")));
		}

		if (ReputationText)
		{
			ReputationText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / 5.0"), GameState->GetReputation())));
		}

		if (VisitorCountText)
		{
			VisitorCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameState->GetVisitorCount())));
		}

		if (WeatherText)
		{
			FName Weather = GameState->GetWeather();
			WeatherText->SetText(Weather.IsNone()
				? FText::FromString(TEXT("Clear"))
				: FText::FromName(Weather));
		}
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooOverviewWidget: ZooGameState not found."));
	}

	UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>();
	if (AnimalMgr && AnimalCountText)
	{
		AnimalCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), AnimalMgr->GetAnimalCount())));
	}

	if (StaffCountText)
	{
		StaffCountText->SetText(FText::FromString(TEXT("0")));
	}
}
