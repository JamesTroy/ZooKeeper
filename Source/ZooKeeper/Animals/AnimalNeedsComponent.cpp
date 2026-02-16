#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"

UAnimalNeedsComponent::UAnimalNeedsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;

	// All needs start fully satisfied.
	Hunger    = 1.0f;
	Thirst    = 1.0f;
	Energy    = 1.0f;
	Health    = 1.0f;
	Happiness = 1.0f;
	Social    = 1.0f;

	// Default decay rates (units per second).
	HungerDecayRate    = 0.005f;
	ThirstDecayRate    = 0.007f;
	EnergyDecayRate    = 0.003f;
	HealthDecayRate    = 0.0f;
	HappinessDecayRate = 0.002f;
	SocialDecayRate    = 0.004f;
}

void UAnimalNeedsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// --- Standard decay ---
	DecayNeed(Hunger,    HungerDecayRate,    DeltaTime, FName("Hunger"));
	DecayNeed(Thirst,    ThirstDecayRate,    DeltaTime, FName("Thirst"));
	DecayNeed(Energy,    EnergyDecayRate,    DeltaTime, FName("Energy"));
	DecayNeed(Health,    HealthDecayRate,    DeltaTime, FName("Health"));
	DecayNeed(Social,    SocialDecayRate,    DeltaTime, FName("Social"));

	// --- Conditional modifiers ---

	// Low energy makes the animal unhappier faster.
	float EffectiveHappinessDecay = HappinessDecayRate;
	if (Energy < 0.2f)
	{
		EffectiveHappinessDecay += 0.004f;
	}
	DecayNeed(Happiness, EffectiveHappinessDecay, DeltaTime, FName("Happiness"));

	// Starvation or dehydration causes health to deteriorate.
	if (Hunger < 0.1f || Thirst < 0.1f)
	{
		const float HealthDamageRate = 0.002f;
		DecayNeed(Health, HealthDamageRate, DeltaTime, FName("Health"));
	}
}

// ---------------------------------------------------------------------------
//  Actions
// ---------------------------------------------------------------------------

void UAnimalNeedsComponent::FeedAnimal(float Amount)
{
	SetNeedValue(Hunger, Hunger + Amount, FName("Hunger"));
	UE_LOG(LogZooKeeper, Verbose, TEXT("%s fed %.2f -> Hunger now %.2f"),
	       *GetOwner()->GetName(), Amount, Hunger);
}

void UAnimalNeedsComponent::GiveWater(float Amount)
{
	SetNeedValue(Thirst, Thirst + Amount, FName("Thirst"));
	UE_LOG(LogZooKeeper, Verbose, TEXT("%s watered %.2f -> Thirst now %.2f"),
	       *GetOwner()->GetName(), Amount, Thirst);
}

void UAnimalNeedsComponent::ReplenishEnergy(float Amount)
{
	SetNeedValue(Energy, Energy + Amount, FName("Energy"));
	UE_LOG(LogZooKeeper, Verbose, TEXT("%s rested %.2f -> Energy now %.2f"),
	       *GetOwner()->GetName(), Amount, Energy);
}

void UAnimalNeedsComponent::Socialize(float Amount)
{
	SetNeedValue(Social, Social + Amount, FName("Social"));
	UE_LOG(LogZooKeeper, Verbose, TEXT("%s socialized %.2f -> Social now %.2f"),
	       *GetOwner()->GetName(), Amount, Social);
}

// ---------------------------------------------------------------------------
//  Queries
// ---------------------------------------------------------------------------

FName UAnimalNeedsComponent::GetMostUrgentNeed() const
{
	struct FNeedEntry
	{
		FName Name;
		float Value;
	};

	const FNeedEntry Needs[] =
	{
		{ FName("Hunger"),    Hunger },
		{ FName("Thirst"),    Thirst },
		{ FName("Energy"),    Energy },
		{ FName("Health"),    Health },
		{ FName("Happiness"), Happiness },
		{ FName("Social"),    Social },
	};

	FName MostUrgent = FName("Hunger");
	float LowestValue = Hunger;

	for (const FNeedEntry& Entry : Needs)
	{
		if (Entry.Value < LowestValue)
		{
			LowestValue = Entry.Value;
			MostUrgent  = Entry.Name;
		}
	}

	return MostUrgent;
}

float UAnimalNeedsComponent::GetOverallWellbeing() const
{
	return (Hunger + Thirst + Energy + Health + Happiness + Social) / 6.0f;
}

bool UAnimalNeedsComponent::IsAnyCritical() const
{
	return Hunger < CriticalThreshold
	    || Thirst < CriticalThreshold
	    || Energy < CriticalThreshold
	    || Health < CriticalThreshold
	    || Happiness < CriticalThreshold
	    || Social < CriticalThreshold;
}

float UAnimalNeedsComponent::GetNeedValue(FName NeedName) const
{
	if (NeedName == FName("Hunger"))    return Hunger;
	if (NeedName == FName("Thirst"))    return Thirst;
	if (NeedName == FName("Energy"))    return Energy;
	if (NeedName == FName("Health"))    return Health;
	if (NeedName == FName("Happiness")) return Happiness;
	if (NeedName == FName("Social"))    return Social;

	UE_LOG(LogZooKeeper, Warning, TEXT("GetNeedValue: unknown need '%s'"), *NeedName.ToString());
	return -1.0f;
}

// ---------------------------------------------------------------------------
//  Internals
// ---------------------------------------------------------------------------

void UAnimalNeedsComponent::DecayNeed(float& NeedValue, float DecayRate, float DeltaTime, FName NeedName)
{
	if (DecayRate <= 0.0f)
	{
		return;
	}

	const float OldValue = NeedValue;
	NeedValue = FMath::Clamp(NeedValue - DecayRate * DeltaTime, 0.0f, 1.0f);

	if (!FMath::IsNearlyEqual(OldValue, NeedValue))
	{
		OnNeedChanged.Broadcast(NeedName, NeedValue);
	}

	if (OldValue >= CriticalThreshold && NeedValue < CriticalThreshold)
	{
		OnNeedCritical.Broadcast(NeedName);
		UE_LOG(LogZooKeeper, Warning, TEXT("%s: need '%s' is now CRITICAL (%.2f)"),
		       *GetOwner()->GetName(), *NeedName.ToString(), NeedValue);
	}
}

void UAnimalNeedsComponent::SetNeedValue(float& NeedValue, float NewValue, FName NeedName)
{
	const float Clamped = FMath::Clamp(NewValue, 0.0f, 1.0f);
	if (!FMath::IsNearlyEqual(NeedValue, Clamped))
	{
		NeedValue = Clamped;
		OnNeedChanged.Broadcast(NeedName, NeedValue);
	}
}
