#include "StaffRosterWidget.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "ZooKeeper.h"

void UStaffRosterWidget::RefreshStaffList()
{
	if (!StaffListPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffRosterWidget: StaffListPanel is not bound."));
		return;
	}

	// Clear existing staff entries.
	StaffListPanel->ClearChildren();

	// TODO: Query the StaffSubsystem (not yet implemented) for all staff members
	// and populate the list panel. The Blueprint child widget will handle creating
	// individual staff entry widgets with name, role, salary, and a fire button.

	// Update summary text fields.
	if (TotalStaffText)
	{
		// Placeholder until StaffSubsystem is implemented.
		TotalStaffText->SetText(FText::FromString(TEXT("0")));
	}

	if (TotalSalaryText)
	{
		// Placeholder until StaffSubsystem is implemented.
		TotalSalaryText->SetText(FText::FromString(TEXT("$0/day")));
	}

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Staff list refreshed."));
}

void UStaffRosterWidget::HireStaffClicked(EStaffType Type)
{
	// TODO: Call StaffSubsystem->HireStaff(Type) once the subsystem is implemented.
	// The subsystem will handle cost validation via EconomySubsystem and spawning.

	const UEnum* StaffTypeEnum = StaticEnum<EStaffType>();
	FString TypeName = StaffTypeEnum ? StaffTypeEnum->GetNameStringByValue(static_cast<int64>(Type)) : TEXT("Unknown");

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Hire button clicked for staff type '%s'."), *TypeName);

	// Refresh the list to reflect any changes.
	RefreshStaffList();
}

void UStaffRosterWidget::FireStaffClicked(int32 StaffID)
{
	// TODO: Call StaffSubsystem->FireStaff(StaffID) once the subsystem is implemented.
	// The subsystem will handle removing the staff member and updating salary totals.

	UE_LOG(LogZooKeeper, Log, TEXT("StaffRosterWidget: Fire button clicked for staff ID %d."), StaffID);

	// Refresh the list to reflect any changes.
	RefreshStaffList();
}
