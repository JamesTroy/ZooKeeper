#include "StaffCharacter.h"
#include "Subsystems/StaffSubsystem.h"
#include "ZooKeeper.h"

AStaffCharacter::AStaffCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StaffType = EStaffType::Zookeeper;
	StaffName = TEXT("Staff Member");
	SkillLevel = 0.5f;
	Salary = 100;
	AssignedEnclosure = nullptr;
}

void AStaffCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		UStaffSubsystem* StaffSubsystem = World->GetSubsystem<UStaffSubsystem>();
		if (StaffSubsystem)
		{
			StaffSubsystem->RegisterStaff(this);
			UE_LOG(LogZooKeeper, Log, TEXT("StaffCharacter [%s] (%s) registered with StaffSubsystem."),
				*StaffName, *UEnum::GetValueAsString(StaffType));
		}
	}
}

void AStaffCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UStaffSubsystem* StaffSubsystem = World->GetSubsystem<UStaffSubsystem>();
		if (StaffSubsystem)
		{
			StaffSubsystem->UnregisterStaff(this);
			UE_LOG(LogZooKeeper, Log, TEXT("StaffCharacter [%s] (%s) unregistered from StaffSubsystem."),
				*StaffName, *UEnum::GetValueAsString(StaffType));
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AStaffCharacter::AssignToEnclosure(AEnclosureActor* Enclosure)
{
	AssignedEnclosure = Enclosure;

	if (Enclosure)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("Staff [%s] assigned to enclosure [%s]."),
			*StaffName, *Enclosure->GetName());
	}
	else
	{
		UE_LOG(LogZooKeeper, Log, TEXT("Staff [%s] unassigned from enclosure."), *StaffName);
	}
}

void AStaffCharacter::PerformDuty()
{
	if (!AssignedEnclosure)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Staff [%s] cannot perform duty: no enclosure assigned."), *StaffName);
		return;
	}

	const float Efficiency = GetEfficiency();

	switch (StaffType)
	{
	case EStaffType::Zookeeper:
		UE_LOG(LogZooKeeper, Log, TEXT("Zookeeper [%s] feeding animals in [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		// Feeding logic would be implemented here, interacting with AnimalNeedsComponent
		break;

	case EStaffType::Veterinarian:
		UE_LOG(LogZooKeeper, Log, TEXT("Veterinarian [%s] treating animals in [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		// Treatment logic would be implemented here
		break;

	case EStaffType::Mechanic:
		UE_LOG(LogZooKeeper, Log, TEXT("Mechanic [%s] repairing structures in [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		// Repair logic would be implemented here
		break;

	case EStaffType::Janitor:
		UE_LOG(LogZooKeeper, Log, TEXT("Janitor [%s] cleaning area around [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		// Cleaning logic would be implemented here
		break;

	case EStaffType::Guide:
		UE_LOG(LogZooKeeper, Log, TEXT("Guide [%s] assisting visitors near [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		// Guide logic would be implemented here
		break;

	default:
		UE_LOG(LogZooKeeper, Warning, TEXT("Staff [%s] has unknown staff type."), *StaffName);
		break;
	}
}

float AStaffCharacter::GetEfficiency() const
{
	// Base efficiency of 0.5, scaling up to 1.0 with skill level
	return 0.5f + (SkillLevel * 0.5f);
}
