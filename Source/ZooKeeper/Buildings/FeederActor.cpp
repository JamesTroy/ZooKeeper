#include "FeederActor.h"
#include "Subsystems/EconomySubsystem.h"
#include "ZooKeeper.h"

AFeederActor::AFeederActor()
{
	FoodType            = EFoodType::Grain;
	MaxCapacity         = 10;
	CurrentStock        = 10;
	RestockCostPerUnit  = 5;
	HungerRestorePerUse = 0.3f;
}

// ---------------------------------------------------------------------------
//  IInteractable
// ---------------------------------------------------------------------------

FText AFeederActor::GetInteractionPrompt_Implementation() const
{
	if (IsEmpty())
	{
		const int32 FullRestockCost = RestockCostPerUnit * MaxCapacity;
		return FText::FromString(FString::Printf(TEXT("Restock Feeder ($%d)"), FullRestockCost));
	}

	return FText::FromString(FString::Printf(TEXT("Feeder: %d/%d"), CurrentStock, MaxCapacity));
}

void AFeederActor::OnInteract_Implementation(AActor* Interactor)
{
	if (CurrentStock >= MaxCapacity)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("Feeder [%s] is already full."), *GetName());
		return;
	}

	const int32 UnitsToAdd = MaxCapacity - CurrentStock;
	const int32 Cost = UnitsToAdd * RestockCostPerUnit;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>();
	if (!EconSys)
	{
		return;
	}

	if (EconSys->TrySpend(Cost, FString::Printf(TEXT("Restock feeder (%d units)"), UnitsToAdd)))
	{
		Restock(UnitsToAdd);
		UE_LOG(LogZooKeeper, Log, TEXT("Feeder [%s] restocked %d units for $%d."), *GetName(), UnitsToAdd, Cost);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Feeder [%s] restock failed — insufficient funds."), *GetName());
	}
}

bool AFeederActor::CanInteract_Implementation(AActor* Interactor) const
{
	return CurrentStock < MaxCapacity;
}

// ---------------------------------------------------------------------------
//  Functions
// ---------------------------------------------------------------------------

void AFeederActor::Restock(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentStock = FMath::Min(CurrentStock + Amount, MaxCapacity);
	OnFeederRestocked.Broadcast(this, CurrentStock);
}

bool AFeederActor::ConsumeFood()
{
	if (CurrentStock <= 0)
	{
		return false;
	}

	CurrentStock--;

	if (CurrentStock <= 0)
	{
		OnFoodDepleted.Broadcast(this);
		UE_LOG(LogZooKeeper, Log, TEXT("Feeder [%s] is now empty."), *GetName());
	}

	return true;
}

bool AFeederActor::IsEmpty() const
{
	return CurrentStock <= 0;
}
