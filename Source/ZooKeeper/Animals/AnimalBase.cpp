#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystems/AnimalManagerSubsystem.h"

#include "Data/ZooDataTypes.h"

// ---------------------------------------------------------------------------
AAnimalBase::AAnimalBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SpeciesID   = NAME_None;
	AnimalName  = TEXT("Unnamed Animal");
	Age         = 0;
	CurrentEnclosure  = nullptr;
	SpeciesDataTable  = nullptr;
	AnimalWalkSpeed   = 200.0f;
	AnimalRunSpeed    = 600.0f;

	// Create the needs component as a default sub-object.
	NeedsComponent = CreateDefaultSubobject<UAnimalNeedsComponent>(TEXT("NeedsComponent"));
}

// ---------------------------------------------------------------------------
//  Lifecycle
// ---------------------------------------------------------------------------

void AAnimalBase::BeginPlay()
{
	Super::BeginPlay();

	// Load species data from the DataTable and apply movement speeds + need decay rates.
	if (FAnimalSpeciesRow* SpeciesRow = GetSpeciesData())
	{
		AnimalWalkSpeed = SpeciesRow->WalkSpeed;
		AnimalRunSpeed  = SpeciesRow->RunSpeed;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = AnimalWalkSpeed;
		}

		// Override need decay rates from species data.
		if (NeedsComponent)
		{
			NeedsComponent->HungerDecayRate    = SpeciesRow->HungerDecayRate;
			NeedsComponent->ThirstDecayRate    = SpeciesRow->ThirstDecayRate;
			NeedsComponent->EnergyDecayRate    = SpeciesRow->EnergyDecayRate;
			NeedsComponent->HealthDecayRate    = SpeciesRow->HealthDecayRate;
			NeedsComponent->HappinessDecayRate = SpeciesRow->HappinessDecayRate;
			NeedsComponent->SocialDecayRate    = SpeciesRow->SocialDecayRate;
		}
	}

	// Bind to the needs component's critical delegate so we can re-broadcast.
	if (NeedsComponent)
	{
		NeedsComponent->OnNeedCritical.AddDynamic(this, &AAnimalBase::HandleNeedCritical);
	}

	// Register with the animal manager subsystem.
	if (UWorld* World = GetWorld())
	{
		if (UAnimalManagerSubsystem* Manager = World->GetSubsystem<UAnimalManagerSubsystem>())
		{
			Manager->RegisterAnimal(this);
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("Animal '%s' (Species: %s) spawned. WalkSpeed=%.0f RunSpeed=%.0f"),
	       *AnimalName, *SpeciesID.ToString(), AnimalWalkSpeed, AnimalRunSpeed);
}

void AAnimalBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the animal manager subsystem.
	if (UWorld* World = GetWorld())
	{
		if (UAnimalManagerSubsystem* Manager = World->GetSubsystem<UAnimalManagerSubsystem>())
		{
			Manager->UnregisterAnimal(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

// ---------------------------------------------------------------------------
//  IInteractable
// ---------------------------------------------------------------------------

FText AAnimalBase::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(FString::Printf(TEXT("Press E to inspect %s"), *AnimalName));
}

void AAnimalBase::OnInteract_Implementation(AActor* Interactor)
{
	OnAnimalInspected.Broadcast(this);
	UE_LOG(LogZooKeeper, Log, TEXT("Player inspected animal '%s'."), *AnimalName);
}

bool AAnimalBase::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

// ---------------------------------------------------------------------------
//  Species Data
// ---------------------------------------------------------------------------

FAnimalSpeciesRow* AAnimalBase::GetSpeciesData() const
{
	if (!SpeciesDataTable)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("GetSpeciesData: SpeciesDataTable is null on '%s'."), *AnimalName);
		return nullptr;
	}

	static const FString ContextString(TEXT("AnimalBase::GetSpeciesData"));
	FAnimalSpeciesRow* Row = SpeciesDataTable->FindRow<FAnimalSpeciesRow>(SpeciesID, ContextString);

	if (!Row)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("GetSpeciesData: row '%s' not found in data table."),
		       *SpeciesID.ToString());
	}

	return Row;
}

// ---------------------------------------------------------------------------
//  Internal Callbacks
// ---------------------------------------------------------------------------

void AAnimalBase::HandleNeedCritical(FName NeedName)
{
	OnAnimalNeedCritical.Broadcast(this, NeedName);
}
