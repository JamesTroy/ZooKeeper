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
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_ZooOverview).
 */
UCLASS(meta = (DisplayName = "Zoo Overview Widget"))
class ZOOKEEPER_API UZooOverviewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Displays the name of the zoo. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* ZooNameText;

	/** Displays the zoo's reputation score. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* ReputationText;

	/** Displays the current number of visitors. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* VisitorCountText;

	/** Displays the total number of animals in the zoo. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* AnimalCountText;

	/** Displays the total number of staff employed. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* StaffCountText;

	/** Displays the current weather condition. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Overview")
	UTextBlock* WeatherText;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/** Pulls data from all subsystems and updates every display field. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Overview")
	void RefreshOverview();

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private:
	/** Accumulator for periodic refresh timing. */
	float RefreshTimer;

	/** How often (in seconds) the overview refreshes. */
	static constexpr float RefreshInterval = 0.5f;
};
