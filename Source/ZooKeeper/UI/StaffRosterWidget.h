#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaffRosterWidget.generated.h"

class UTextBlock;
class UPanelWidget;

/**
 * EStaffType
 *
 * Enumerates the different types of staff that can be hired in the zoo.
 * Forward-declared here as it is not yet defined elsewhere in the project.
 */
UENUM(BlueprintType)
enum class EStaffType : uint8
{
	Zookeeper		UMETA(DisplayName = "Zookeeper"),
	Veterinarian	UMETA(DisplayName = "Veterinarian"),
	Janitor			UMETA(DisplayName = "Janitor"),
	Mechanic		UMETA(DisplayName = "Mechanic"),
	Guide			UMETA(DisplayName = "Guide")
};

/**
 * UStaffRosterWidget
 *
 * Widget that displays the zoo's staff roster, including total headcount,
 * total salary expenditure, and a scrollable list of individual staff members.
 * Provides hire and fire actions that interact with the staff management subsystem.
 *
 * Designed as a C++ base class for a Blueprint child widget (WBP_StaffRoster).
 */
UCLASS(meta = (DisplayName = "Staff Roster Widget"))
class ZOOKEEPER_API UStaffRosterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------
	//  Bound Widgets (must exist in Blueprint child)
	// -------------------------------------------------------------------

	/** Displays the total number of staff employed. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Staff")
	UTextBlock* TotalStaffText;

	/** Displays the total salary cost per day. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Staff")
	UTextBlock* TotalSalaryText;

	/** Scrollable panel containing individual staff entries. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Zoo|Staff")
	UPanelWidget* StaffListPanel;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/** Refreshes the entire staff list from the staff management subsystem. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void RefreshStaffList();

	/**
	 * Handles the hire button being clicked for a specific staff type.
	 * @param Type  The type of staff member to hire.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void HireStaffClicked(EStaffType Type);

	/**
	 * Handles the fire button being clicked for a specific staff member.
	 * @param StaffID  The unique identifier of the staff member to fire.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void FireStaffClicked(int32 StaffID);
};
