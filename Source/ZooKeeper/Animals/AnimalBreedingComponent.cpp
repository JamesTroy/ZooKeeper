#include "AnimalBreedingComponent.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "Subsystems/TimeSubsystem.h"
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
		// Convert real-time DeltaTime into game-day units using the TimeSubsystem's scale.
		// GameTimeScale converts real seconds to game seconds; a game day is 86400 game seconds.
		float TimeScale = 60.0f; // Default: 1 real second = 1 game minute
		if (UWorld* World = GetWorld())
		{
			if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
			{
				TimeScale = TimeSys->GameTimeScale;
			}
		}

		const float GameSecondsElapsed = DeltaTime * TimeScale;
		const float GameDayDelta = GameSecondsElapsed / 86400.0f;
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
