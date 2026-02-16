#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/StaffSubsystem.h"
#include "StaffRosterWidget.generated.h"

class UTextBlock;
class UPanelWidget;

/**
 * UStaffRosterWidget
 *
 * Widget that displays the zoo's staff roster, including total headcount,
 * total salary expenditure, and a scrollable list of individual staff members.
 * Builds its widget tree entirely in C++ — no Blueprint asset required.
 */
UCLASS(meta = (DisplayName = "Staff Roster Widget"))
class ZOOKEEPER_API UStaffRosterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void RefreshStaffList();

	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void HireStaffClicked(EStaffType Type);

	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void FireStaffClicked(int32 StaffID);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TObjectPtr<UTextBlock> TotalStaffText;

	UPROPERTY()
	TObjectPtr<UTextBlock> TotalSalaryText;

	UPROPERTY()
	TObjectPtr<UPanelWidget> StaffListPanel;
};
