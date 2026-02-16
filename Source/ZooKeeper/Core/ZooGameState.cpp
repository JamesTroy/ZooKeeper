#include "ZooGameState.h"

#include "ZooKeeper.h"
#include "Net/UnrealNetwork.h"

AZooGameState::AZooGameState()
	: CurrentFunds(0)
	, ZooReputation(0.0f)
	, CurrentDay(1)
	, CurrentWeather(NAME_None)
	, VisitorCount(0)
{
}

void AZooGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZooGameState, CurrentFunds);
	DOREPLIFETIME(AZooGameState, ZooReputation);
	DOREPLIFETIME(AZooGameState, CurrentDay);
	DOREPLIFETIME(AZooGameState, CurrentWeather);
	DOREPLIFETIME(AZooGameState, VisitorCount);
}

// ---------------------------------------------------------------------------
//  Economy
// ---------------------------------------------------------------------------

void AZooGameState::AddFunds(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooGameState::AddFunds - Attempted to add non-positive amount: %d"), Amount);
		return;
	}

	CurrentFunds += Amount;
	OnFundsChanged.Broadcast(CurrentFunds, Amount);

	UE_LOG(LogZooKeeper, Verbose, TEXT("ZooGameState::AddFunds +%d -> %d"), Amount, CurrentFunds);
}

bool AZooGameState::RemoveFunds(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooGameState::RemoveFunds - Attempted to remove non-positive amount: %d"), Amount);
		return false;
	}

	if (CurrentFunds < Amount)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("ZooGameState::RemoveFunds - Insufficient funds: Have %d, Need %d"), CurrentFunds, Amount);
		return false;
	}

	CurrentFunds -= Amount;
	OnFundsChanged.Broadcast(CurrentFunds, -Amount);

	UE_LOG(LogZooKeeper, Verbose, TEXT("ZooGameState::RemoveFunds -%d -> %d"), Amount, CurrentFunds);
	return true;
}

// ---------------------------------------------------------------------------
//  Reputation
// ---------------------------------------------------------------------------

void AZooGameState::SetReputation(float NewReputation)
{
	const float ClampedReputation = FMath::Clamp(NewReputation, 0.0f, 5.0f);

	if (!FMath::IsNearlyEqual(ZooReputation, ClampedReputation))
	{
		const float OldReputation = ZooReputation;
		ZooReputation = ClampedReputation;
		OnReputationChanged.Broadcast(ZooReputation, OldReputation);

		UE_LOG(LogZooKeeper, Verbose, TEXT("ZooGameState::SetReputation %.2f -> %.2f"), OldReputation, ZooReputation);
	}
}

// ---------------------------------------------------------------------------
//  Day / Time
// ---------------------------------------------------------------------------

void AZooGameState::AdvanceDay()
{
	++CurrentDay;
	OnDayChanged.Broadcast(CurrentDay);

	UE_LOG(LogZooKeeper, Log, TEXT("ZooGameState::AdvanceDay -> %s"), *GetFormattedDate());
}

FString AZooGameState::GetFormattedDate() const
{
	return FString::Printf(TEXT("Day %d"), CurrentDay);
}

// ---------------------------------------------------------------------------
//  Weather
// ---------------------------------------------------------------------------

void AZooGameState::SetWeather(FName NewWeather)
{
	if (CurrentWeather != NewWeather)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("ZooGameState::SetWeather %s -> %s"), *CurrentWeather.ToString(), *NewWeather.ToString());
		CurrentWeather = NewWeather;
	}
}

// ---------------------------------------------------------------------------
//  Visitors
// ---------------------------------------------------------------------------

void AZooGameState::SetVisitorCount(int32 Count)
{
	VisitorCount = FMath::Max(0, Count);
}
