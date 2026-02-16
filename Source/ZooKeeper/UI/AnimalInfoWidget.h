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
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Animal Info Widget"))
class ZOOKEEPER_API UAnimalInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UUserWidget Interface

private:
	void BuildWidgetTree();
	UProgressBar* CreateNeedRow(class UVerticalBox* Parent, const FString& LabelStr, const FLinearColor& BarColor);

	/** Callback for OnNeedChanged delegate from the animal's NeedsComponent. */
	UFUNCTION()
	void HandleNeedChanged(FName NeedName, float NewValue);

	// -------------------------------------------------------------------
	//  Widget references (built programmatically)
	// -------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<UTextBlock> AnimalNameText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SpeciesText;

	UPROPERTY()
	TObjectPtr<UTextBlock> AgeText;

	UPROPERTY()
	TObjectPtr<UProgressBar> HungerBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> ThirstBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> EnergyBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> HappinessBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> SocialBar;

	/** Weak reference to the animal currently being displayed. */
	UPROPERTY()
	TWeakObjectPtr<AAnimalBase> CurrentAnimal;
};
