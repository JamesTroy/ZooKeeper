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
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Zoo HUD Widget"))
class ZOOKEEPER_API UZooHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private:
	/** Builds the UMG widget tree programmatically. */
	void BuildWidgetTree();

	/** Callback bound to UTimeSubsystem::OnTimeOfDayChanged. */
	UFUNCTION()
	void HandleTimeOfDayChanged(float NewTime);

	/** Callback bound to UTimeSubsystem::OnDayChanged. */
	UFUNCTION()
	void HandleDayChanged(int32 NewDay);

	/** Callback bound to UEconomySubsystem::OnFundsChanged. */
	UFUNCTION()
	void HandleFundsChanged(int32 NewBalance);

	// -------------------------------------------------------------------
	//  Widget references (built programmatically)
	// -------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY()
	TObjectPtr<UTextBlock> DayText;

	UPROPERTY()
	TObjectPtr<UTextBlock> FundsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> InteractionPromptText;

	UPROPERTY()
	TObjectPtr<UImage> CrosshairImage;

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
