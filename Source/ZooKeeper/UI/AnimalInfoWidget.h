#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimalInfoWidget.generated.h"

class UTextBlock;
class UProgressBar;
class AAnimalBase;

/**
 * UAnimalInfoWidget
 *
 * Widget that displays detailed information about a selected animal,
 * including its name, species, age, and all need bars (hunger, thirst,
 * energy, health, happiness, social). Updates automatically each tick
 * when an animal is assigned.
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_AnimalInfo).
 */
UCLASS(meta = (DisplayName = "Animal Info Widget"))
class ZOOKEEPER_API UAnimalInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Displays the animal's individual name. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UTextBlock* AnimalNameText;

	/** Displays the animal's species. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UTextBlock* SpeciesText;

	/** Displays the animal's age in days. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UTextBlock* AgeText;

	/** Progress bar showing the animal's hunger level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* HungerBar;

	/** Progress bar showing the animal's thirst level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* ThirstBar;

	/** Progress bar showing the animal's energy level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* EnergyBar;

	/** Progress bar showing the animal's health level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* HealthBar;

	/** Progress bar showing the animal's happiness level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* HappinessBar;

	/** Progress bar showing the animal's social need level (0-1). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|AnimalInfo")
	UProgressBar* SocialBar;

	// -------------------------------------------------------------------
	//  Animal Data
	// -------------------------------------------------------------------

	/**
	 * Sets the animal whose data this widget will display.
	 * @param Animal  The animal to display. Pass nullptr to clear.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|AnimalInfo")
	void SetAnimalData(AAnimalBase* Animal);

	/** Updates all need bars from the currently stored animal's NeedsComponent. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|AnimalInfo")
	void UpdateNeedBars();

	/** Clears the currently stored animal reference and resets the display. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|AnimalInfo")
	void ClearAnimalData();

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private:
	/** Weak reference to the animal currently being displayed. */
	UPROPERTY()
	TWeakObjectPtr<AAnimalBase> CurrentAnimal;
};
