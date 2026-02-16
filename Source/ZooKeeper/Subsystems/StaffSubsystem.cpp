#include "StaffSubsystem.h"
#include "Staff/StaffCharacter.h"
#include "Buildings/EnclosureActor.h"
#include "ZooKeeper.h"

bool UStaffSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UStaffSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	NextStaffID = 1;

	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem::Initialize"));
}

void UStaffSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem::Deinitialize - %d staff records at shutdown."), StaffRecords.Num());

	StaffRecords.Empty();

	Super::Deinitialize();
}

int32 UStaffSubsystem::HireStaff(EStaffType Type, FString Name)
{
	if (Name.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffSubsystem::HireStaff - Empty name provided."));
		return INDEX_NONE;
	}

	FStaffRecord NewRecord;
	NewRecord.StaffID = NextStaffID++;
	NewRecord.Name = MoveTemp(Name);
	NewRecord.Type = Type;
	NewRecord.Salary = GetDefaultSalary(Type);
	NewRecord.Skill = 0.5f;
	NewRecord.AssignedEnclosure = nullptr;

	StaffRecords.Add(NewRecord);
	OnStaffHired.Broadcast(NewRecord.StaffID);

	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem - Hired '%s' (ID: %d, Type: %d, Salary: %d). Total staff: %d"),
		*NewRecord.Name, NewRecord.StaffID, static_cast<int32>(NewRecord.Type), NewRecord.Salary, StaffRecords.Num());

	return NewRecord.StaffID;
}

void UStaffSubsystem::FireStaff(int32 StaffID)
{
	const int32 Index = FindStaffIndex(StaffID);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffSubsystem::FireStaff - No staff member found with ID: %d"), StaffID);
		return;
	}

	const FString FiredName = StaffRecords[Index].Name;
	StaffRecords.RemoveAt(Index);
	OnStaffFired.Broadcast(StaffID);

	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem - Fired '%s' (ID: %d). Total staff: %d"),
		*FiredName, StaffID, StaffRecords.Num());
}

void UStaffSubsystem::AssignToEnclosure(int32 StaffID, AEnclosureActor* Enclosure)
{
	const int32 Index = FindStaffIndex(StaffID);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("StaffSubsystem::AssignToEnclosure - No staff member found with ID: %d"), StaffID);
		return;
	}

	StaffRecords[Index].AssignedEnclosure = Enclosure;

	if (Enclosure)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem - Assigned '%s' (ID: %d) to enclosure."),
			*StaffRecords[Index].Name, StaffID);
	}
	else
	{
		UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem - Unassigned '%s' (ID: %d) from enclosure."),
			*StaffRecords[Index].Name, StaffID);
	}
}

int32 UStaffSubsystem::GetStaffCount() const
{
	return StaffRecords.Num();
}

int32 UStaffSubsystem::GetDailySalaryCost() const
{
	int32 TotalCost = 0;
	for (const FStaffRecord& Record : StaffRecords)
	{
		TotalCost += Record.Salary;
	}
	return TotalCost;
}

FStaffRecord UStaffSubsystem::GetStaffRecord(int32 StaffID) const
{
	const int32 Index = FindStaffIndex(StaffID);
	if (Index != INDEX_NONE)
	{
		return StaffRecords[Index];
	}
	return FStaffRecord();
}

int32 UStaffSubsystem::FindStaffIndex(int32 StaffID) const
{
	for (int32 i = 0; i < StaffRecords.Num(); ++i)
	{
		if (StaffRecords[i].StaffID == StaffID)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UStaffSubsystem::RegisterStaff(AStaffCharacter* StaffCharacter)
{
	if (!StaffCharacter)
	{
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem::RegisterStaff - '%s'"), *StaffCharacter->StaffName);
}

void UStaffSubsystem::UnregisterStaff(AStaffCharacter* StaffCharacter)
{
	if (!StaffCharacter)
	{
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("StaffSubsystem::UnregisterStaff - '%s'"), *StaffCharacter->StaffName);
}

int32 UStaffSubsystem::GetDefaultSalary(EStaffType Type)
{
	switch (Type)
	{
	case EStaffType::Zookeeper:     return 100;
	case EStaffType::Veterinarian:  return 200;
	case EStaffType::Janitor:       return 75;
	case EStaffType::Mechanic:      return 125;
	case EStaffType::Guide:         return 90;
	default:                        return 100;
	}
}
