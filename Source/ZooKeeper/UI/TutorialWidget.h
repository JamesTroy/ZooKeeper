#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

class UTextBlock;

/**
 * FTutorialStep
 *
 * A single tutorial tooltip step with a title and body message.
 */
USTRUCT(BlueprintType)
struct FTutorialStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Tutorial")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Tutorial")
	FString Message;
};

/** Broadcast when the tutorial is completed or skipped. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialCompleted);

/**
 * UTutorialWidget
 *
 * Sequential tooltip popup widget for the new player tutorial.
 * Displays steps one at a time with Next and Skip buttons.
 * Builds its widget tree entirely in C++ -- no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Tutorial Widget"))
class ZOOKEEPER_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Starts the tutorial from the first step. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tutorial")
	void StartTutorial();

	/** Advances to the next tutorial step. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tutorial")
	void NextStep();

	/** Skips the entire tutorial. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tutorial")
	void SkipTutorial();

	/** Returns whether the tutorial has been completed. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Tutorial")
	bool IsTutorialComplete() const { return bIsComplete; }

	/** Fired when the tutorial finishes (completed or skipped). */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|Tutorial")
	FOnTutorialCompleted OnTutorialCompleted;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/** Tutorial steps (configured in Blueprint or set in C++ defaults). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Tutorial")
	TArray<FTutorialStep> Steps;

private:
	/** Shows the current step. */
	void DisplayCurrentStep();

	/** Completes the tutorial and fires the delegate. */
	void FinishTutorial();

	/** Initializes the default tutorial steps if none are configured. */
	void EnsureDefaultSteps();

	UPROPERTY()
	TObjectPtr<UTextBlock> StepTitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> StepMessageText;

	UPROPERTY()
	TObjectPtr<UTextBlock> StepCounterText;

	int32 CurrentStepIndex = 0;
	bool bIsComplete = false;
};
