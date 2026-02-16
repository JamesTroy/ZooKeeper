#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZooHUDWidget.generated.h"

class UTextBlock;
class UImage;
class UTimeSubsystem;
class UEconomySubsystem;
class UInteractionComponent;

/**
 * UZooHUDWidget
 *
 * Main heads-up display widget for the Zoo Keeper game. Shows the current
 * time of day, day number, player funds, interaction prompts, and a crosshair.
 * Designed as a C++ base class for a Blueprint child widget (WBP_ZooHUD)
 * that provides the visual layout. All bound widgets use meta=(BindWidget)
 * so the Blueprint child must contain matching named widgets.
 */
UCLASS(meta = (DisplayName = "Zoo HUD Widget"))
class ZOOKEEPER_API UZooHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Displays the current in-game time (e.g. "14:35"). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|HUD")
	UTextBlock* TimeText;

	/** Displays the current in-game day (e.g. "Day 7"). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|HUD")
	UTextBlock* DayText;

	/** Displays the player's current funds. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|HUD")
	UTextBlock* FundsText;

	/** Displays a contextual interaction prompt when the player looks at an interactable. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|HUD")
	UTextBlock* InteractionPromptText;

	/** Crosshair image displayed at the center of the screen. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|HUD")
	UImage* CrosshairImage;

	// -------------------------------------------------------------------
	//  Display Update Functions
	// -------------------------------------------------------------------

	/** Updates the time display with the given text. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void UpdateTimeDisplay(FText Time);

	/** Updates the day display with the given text. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void UpdateDayDisplay(FText Day);

	/** Updates the funds display with the given amount. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void UpdateFundsDisplay(int32 Funds);

	/** Shows the interaction prompt with the given text. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void ShowInteractionPrompt(FText Prompt);

	/** Hides the interaction prompt. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void HideInteractionPrompt();

	/** Sets the crosshair image visibility. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|HUD")
	void SetCrosshairVisible(bool bVisible);

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private:
	/** Callback bound to UTimeSubsystem::OnTimeOfDayChanged. */
	UFUNCTION()
	void HandleTimeOfDayChanged(float NewTime);

	/** Callback bound to UTimeSubsystem::OnDayChanged. */
	UFUNCTION()
	void HandleDayChanged(int32 NewDay);

	/** Callback bound to UEconomySubsystem::OnFundsChanged. */
	UFUNCTION()
	void HandleFundsChanged(int32 NewBalance);

	/** Cached reference to the time subsystem. */
	UPROPERTY()
	TWeakObjectPtr<UTimeSubsystem> TimeSubsystem;

	/** Cached reference to the economy subsystem. */
	UPROPERTY()
	TWeakObjectPtr<UEconomySubsystem> EconomySubsystem;

	/** Cached reference to the player's interaction component. */
	UPROPERTY()
	TWeakObjectPtr<UInteractionComponent> CachedInteractionComp;
};
