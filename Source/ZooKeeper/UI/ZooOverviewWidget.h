#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZooOverviewWidget.generated.h"

class UTextBlock;

/**
 * UZooOverviewWidget
 *
 * Widget that provides a high-level overview of the zoo's current state,
 * including the zoo name, reputation, visitor count, animal count, staff
 * count, and current weather. Refreshes periodically every 0.5 seconds.
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Zoo Overview Widget"))
class ZOOKEEPER_API UZooOverviewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|Overview")
	void RefreshOverview();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<UTextBlock> ZooNameText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ReputationText;

	UPROPERTY()
	TObjectPtr<UTextBlock> VisitorCountText;

	UPROPERTY()
	TObjectPtr<UTextBlock> AnimalCountText;

	UPROPERTY()
	TObjectPtr<UTextBlock> StaffCountText;

	UPROPERTY()
	TObjectPtr<UTextBlock> WeatherText;

	float RefreshTimer;

	static constexpr float RefreshInterval = 0.5f;
};
