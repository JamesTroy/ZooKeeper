#include "ZooBuildingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ZooKeeper/ZooKeeper.h"
#include "ZooKeeper/Subsystems/BuildingManagerSubsystem.h"

AZooBuildingActor::AZooBuildingActor()
	: PurchaseCost(0)
	, Condition(1.0f)
	, MaintenanceCostPerDay(0.0f)
	, bIsPlaced(false)
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root mesh component
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(BuildingMesh);

	// Create collision box attached to root
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(BuildingMesh);
	CollisionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Generate a persistent save ID
	SaveID = FGuid::NewGuid();
}

void AZooBuildingActor::BeginPlay()
{
	Super::BeginPlay();

	// Register with the BuildingManagerSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UBuildingManagerSubsystem* BuildingManager = World->GetSubsystem<UBuildingManagerSubsystem>())
		{
			BuildingManager->RegisterBuilding(this);
			UE_LOG(LogZooKeeper, Log, TEXT("Building '%s' registered with BuildingManagerSubsystem."), *BuildingName);
		}
		else
		{
			UE_LOG(LogZooKeeper, Warning, TEXT("Building '%s': BuildingManagerSubsystem not found."), *BuildingName);
		}
	}
}

// -------------------------------------------------------------------
//  IInteractable Implementation
// -------------------------------------------------------------------

FText AZooBuildingActor::GetInteractionPrompt_Implementation() const
{
	const int32 ConditionPercent = FMath::RoundToInt(Condition * 100.0f);
	return FText::FromString(FString::Printf(TEXT("%s (Condition: %d%%)"), *BuildingName, ConditionPercent));
}

void AZooBuildingActor::OnInteract_Implementation(AActor* Interactor)
{
	UE_LOG(LogZooKeeper, Log, TEXT("Building '%s' interacted by '%s'."),
		*BuildingName, Interactor ? *Interactor->GetName() : TEXT("None"));

	OnBuildingInteracted.Broadcast(this, Interactor);
}

bool AZooBuildingActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

// -------------------------------------------------------------------
//  IZooSaveable Implementation
// -------------------------------------------------------------------

void AZooBuildingActor::SerializeSaveData_Implementation(FArchive& Ar)
{
	// Serialize transform
	FTransform CurrentTransform = GetActorTransform();
	Ar << CurrentTransform;

	// Serialize condition
	Ar << Condition;

	// Serialize building ID
	Ar << BuildingID;

	// On load, apply the deserialized transform
	if (Ar.IsLoading())
	{
		SetActorTransform(CurrentTransform);
		UE_LOG(LogZooKeeper, Log, TEXT("Building '%s': Loaded save data (Condition: %.2f)."), *BuildingName, Condition);
	}
}

FGuid AZooBuildingActor::GetSaveID_Implementation() const
{
	return SaveID;
}

// -------------------------------------------------------------------
//  Building Functions
// -------------------------------------------------------------------

void AZooBuildingActor::DegradeCondition(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	const float OldCondition = Condition;
	Condition = FMath::Clamp(Condition - Amount, 0.0f, 1.0f);

	if (!FMath::IsNearlyEqual(OldCondition, Condition))
	{
		OnConditionChanged.Broadcast(this, Condition);
		UE_LOG(LogZooKeeper, Verbose, TEXT("Building '%s': Condition degraded from %.2f to %.2f."),
			*BuildingName, OldCondition, Condition);
	}
}

void AZooBuildingActor::Repair()
{
	const float OldCondition = Condition;
	Condition = 1.0f;

	if (!FMath::IsNearlyEqual(OldCondition, Condition))
	{
		OnConditionChanged.Broadcast(this, Condition);
		UE_LOG(LogZooKeeper, Log, TEXT("Building '%s': Repaired from %.2f to 1.0."), *BuildingName, OldCondition);
	}
}

int32 AZooBuildingActor::GetMaintenanceCost() const
{
	return FMath::RoundToInt(MaintenanceCostPerDay);
}
