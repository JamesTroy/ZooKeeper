#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UTextBlock;

/**
 * UPauseMenuWidget
 *
 * Widget for the in-game pause menu. Provides Resume, Save, Load, and Quit options.
 * Builds its widget tree entirely in C++ -- no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Pause Menu Widget"))
class ZOOKEEPER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnResumeClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnSaveClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnLoadClicked();

	UFUNCTION(BlueprintCallable, Category = "Zoo|UI")
	void OnQuitClicked();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
