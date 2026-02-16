#include "ZooOverviewWidget.h"

#include "Components/TextBlock.h"
#include "Core/ZooGameState.h"
#include "Subsystems/AnimalManagerSubsystem.h"
#include "ZooKeeper.h"

void UZooOverviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshTimer = 0.0f;

	// Perform an initial refresh.
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

	// Pull data from the ZooGameState.
	AZooGameState* GameState = Cast<AZooGameState>(World->GetGameState());
	if (GameState)
	{
		if (ZooNameText)
		{
			// The zoo name could come from a save file or game instance.
			// For now, display a default until a naming system is implemented.
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

	// Pull animal count from the AnimalManagerSubsystem.
	UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>();
	if (AnimalMgr && AnimalCountText)
	{
		AnimalCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), AnimalMgr->GetAnimalCount())));
	}

	// Staff count from the StaffSubsystem (not yet implemented).
	if (StaffCountText)
	{
		// TODO: Query StaffSubsystem for total staff count once implemented.
		StaffCountText->SetText(FText::FromString(TEXT("0")));
	}
}
