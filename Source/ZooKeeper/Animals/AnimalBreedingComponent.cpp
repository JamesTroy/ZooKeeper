#include "AnimalBreedingComponent.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"

UAnimalBreedingComponent::UAnimalBreedingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;

	Sex              = EAnimalSex::Male;
	MaturityAge      = 3.0f;
	GestationPeriod  = 5.0f;
	bIsPregnant      = false;
	GestationProgress = 0.0f;
}

void UAnimalBreedingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPregnant)
	{
		// Convert real-time DeltaTime into game-day units.
		// Using the same convention as TimeSubsystem: 1 real second = 1 game minute
		// => 1 game-day = 24 * 60 real seconds = 1440 seconds.
		// However, the actual time scale may vary, so we use a simple conversion here.
		// Callers can also drive gestation directly via TickGestation().
		const float GameDayDelta = DeltaTime / 1440.0f;
		TickGestation(GameDayDelta);
	}
}

bool UAnimalBreedingComponent::CanBreed() const
{
	AAnimalBase* Animal = Cast<AAnimalBase>(GetOwner());
	if (!Animal)
	{
		return false;
	}

	// Must be old enough.
	if (static_cast<float>(Animal->Age) < MaturityAge)
	{
		return false;
	}

	// Must not already be pregnant.
	if (bIsPregnant)
	{
		return false;
	}

	// Needs should not be critically low (overall wellbeing > 0.4).
	if (Animal->NeedsComponent && Animal->NeedsComponent->GetOverallWellbeing() <= 0.4f)
	{
		return false;
	}

	return true;
}

bool UAnimalBreedingComponent::TryBreed(UAnimalBreedingComponent* Partner)
{
	if (!Partner)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("TryBreed: null partner."));
		return false;
	}

	// Both must be able to breed.
	if (!CanBreed() || !Partner->CanBreed())
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("TryBreed: one or both animals cannot breed."));
		return false;
	}

	// Must be opposite sex.
	if (Sex == Partner->Sex)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("TryBreed: same-sex pairing."));
		return false;
	}

	// Must be the same species.
	AAnimalBase* MyAnimal = Cast<AAnimalBase>(GetOwner());
	AAnimalBase* PartnerAnimal = Cast<AAnimalBase>(Partner->GetOwner());
	if (!MyAnimal || !PartnerAnimal)
	{
		return false;
	}

	if (MyAnimal->SpeciesID != PartnerAnimal->SpeciesID)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("TryBreed: different species (%s vs %s)."),
		       *MyAnimal->SpeciesID.ToString(), *PartnerAnimal->SpeciesID.ToString());
		return false;
	}

	// Identify the female and make her pregnant.
	UAnimalBreedingComponent* Female = (Sex == EAnimalSex::Female) ? this : Partner;
	Female->bIsPregnant      = true;
	Female->GestationProgress = 0.0f;

	AAnimalBase* FemaleAnimal = Cast<AAnimalBase>(Female->GetOwner());
	UE_LOG(LogZooKeeper, Log, TEXT("Breeding successful! '%s' is now pregnant."),
	       FemaleAnimal ? *FemaleAnimal->AnimalName : TEXT("Unknown"));

	return true;
}

void UAnimalBreedingComponent::TickGestation(float DeltaTime)
{
	if (!bIsPregnant)
	{
		return;
	}

	GestationProgress += DeltaTime;

	if (GestationProgress >= GestationPeriod)
	{
		bIsPregnant       = false;
		GestationProgress = 0.0f;

		AAnimalBase* Parent = Cast<AAnimalBase>(GetOwner());
		OnBabyBorn.Broadcast(Parent);

		UE_LOG(LogZooKeeper, Log, TEXT("Baby born! Parent: '%s'."),
		       Parent ? *Parent->AnimalName : TEXT("Unknown"));
	}
}
