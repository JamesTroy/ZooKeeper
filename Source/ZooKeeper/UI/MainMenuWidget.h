#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UTextBlock;

/**
 * UMainMenuWidget
 *
 * Full-screen main menu widget with New Game, Continue, Load Game, Settings, and Quit options.
 * Builds its widget tree entirely in C++ -- no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Main Menu Widget"))
class ZOOKEEPER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnNewGameClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnContinueClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnLoadGameClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnSettingsClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnQuitClicked();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	/** Text block for the Continue option -- hidden when no save exists. */
	UPROPERTY()
	TObjectPtr<UTextBlock> ContinueText;
};
