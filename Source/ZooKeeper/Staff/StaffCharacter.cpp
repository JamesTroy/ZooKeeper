#include "StaffCharacter.h"
#include "Subsystems/StaffSubsystem.h"
#include "Buildings/EnclosureActor.h"
#include "Buildings/FeederActor.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
#include "Kismet/GameplayStatics.h"
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
	{
		// Find empty feeders in the enclosure and restock them.
		TArray<AActor*> NearbyActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFeederActor::StaticClass(), NearbyActors);

		for (AActor* Actor : NearbyActors)
		{
			AFeederActor* Feeder = Cast<AFeederActor>(Actor);
			if (Feeder && Feeder->IsEmpty())
			{
				const float Dist = FVector::Dist(Feeder->GetActorLocation(), AssignedEnclosure->GetActorLocation());
				if (Dist < 3000.0f) // Within reasonable range of enclosure
				{
					Feeder->Restock(FMath::RoundToInt(Feeder->MaxCapacity * Efficiency));
					UE_LOG(LogZooKeeper, Log, TEXT("Zookeeper [%s] restocked feeder near [%s]."),
						*StaffName, *AssignedEnclosure->GetName());
					return;
				}
			}
		}

		UE_LOG(LogZooKeeper, Verbose, TEXT("Zookeeper [%s] found no empty feeders near [%s]."),
			*StaffName, *AssignedEnclosure->GetName());
		break;
	}

	case EStaffType::Veterinarian:
	{
		// Find sick animals (Health < 0.5) in the enclosure and heal them.
		for (AAnimalBase* Animal : AssignedEnclosure->ContainedAnimals)
		{
			if (!Animal || !Animal->NeedsComponent)
			{
				continue;
			}

			const float Health = Animal->NeedsComponent->GetNeedValue(FName("Health"));
			if (Health < 0.5f)
			{
				const float HealAmount = 0.3f * Efficiency;
				Animal->NeedsComponent->ModifyNeed(FName("Health"), HealAmount);
				UE_LOG(LogZooKeeper, Log, TEXT("Veterinarian [%s] treated animal [%s] in [%s] (healed %.2f)."),
					*StaffName, *Animal->AnimalName, *AssignedEnclosure->GetName(), HealAmount);
				return;
			}
		}

		UE_LOG(LogZooKeeper, Verbose, TEXT("Veterinarian [%s] found no sick animals in [%s]."),
			*StaffName, *AssignedEnclosure->GetName());
		break;
	}

	case EStaffType::Mechanic:
	{
		// Repair damaged enclosure (Condition < 0.5).
		if (AssignedEnclosure->Condition < 0.5f)
		{
			const float RepairAmount = 0.2f * Efficiency;
			AssignedEnclosure->Condition = FMath::Clamp(AssignedEnclosure->Condition + RepairAmount, 0.0f, 1.0f);
			UE_LOG(LogZooKeeper, Log, TEXT("Mechanic [%s] repaired [%s] (condition now: %.2f)."),
				*StaffName, *AssignedEnclosure->GetName(), AssignedEnclosure->Condition);
			return;
		}

		UE_LOG(LogZooKeeper, Verbose, TEXT("Mechanic [%s] found no damaged structures in [%s]."),
			*StaffName, *AssignedEnclosure->GetName());
		break;
	}

	case EStaffType::Janitor:
	{
		// Janitors boost visitor satisfaction by keeping the area clean.
		// For now, just log the action — cleanliness system can be added later.
		UE_LOG(LogZooKeeper, Log, TEXT("Janitor [%s] cleaned area around [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		break;
	}

	case EStaffType::Guide:
	{
		// Guides boost visitor satisfaction by providing tours.
		UE_LOG(LogZooKeeper, Log, TEXT("Guide [%s] assisting visitors near [%s] (efficiency: %.2f)."),
			*StaffName, *AssignedEnclosure->GetName(), Efficiency);
		break;
	}

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
