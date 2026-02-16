#include "ZooRatingSubsystem.h"
#include "AnimalManagerSubsystem.h"
#include "VisitorSubsystem.h"
#include "StaffSubsystem.h"
#include "BuildingManagerSubsystem.h"
#include "TimeSubsystem.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
#include "Buildings/EnclosureActor.h"
#include "ZooKeeper.h"

bool UZooRatingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UZooRatingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentRating = 0.0f;
	AnimalDiversityScore = 0.0f;
	AnimalHappinessScore = 0.0f;
	VisitorSatisfactionScore = 0.0f;
	EnclosureQualityScore = 0.0f;
	AmenityScore = 0.0f;

	// Subscribe to day changes to recalculate daily.
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->OnDayChanged.AddDynamic(this, &UZooRatingSubsystem::HandleDayChanged);
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooRatingSubsystem::Initialize"));
}

void UZooRatingSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("ZooRatingSubsystem::Deinitialize - Final rating: %.2f"), CurrentRating);
	Super::Deinitialize();
}

void UZooRatingSubsystem::RecalculateRating()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// --- Animal Diversity (0-1): based on unique species count ---
	AnimalDiversityScore = 0.0f;
	if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
	{
		TSet<FName> UniqueSpecies;
		TArray<AAnimalBase*> Animals = AnimalMgr->GetAnimalsInEnclosure(nullptr);
		for (AAnimalBase* Animal : Animals)
		{
			if (Animal && !Animal->SpeciesID.IsNone())
			{
				UniqueSpecies.Add(Animal->SpeciesID);
			}
		}
		// Score: 1 species = 0.2, 5+ species = 1.0
		AnimalDiversityScore = FMath::Clamp(static_cast<float>(UniqueSpecies.Num()) / 5.0f, 0.0f, 1.0f);
	}

	// --- Animal Happiness (0-1): average happiness across all animals ---
	AnimalHappinessScore = 0.5f;
	if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
	{
		TArray<AAnimalBase*> Animals = AnimalMgr->GetAnimalsInEnclosure(nullptr);
		if (Animals.Num() > 0)
		{
			float TotalHappiness = 0.0f;
			int32 Count = 0;
			for (AAnimalBase* Animal : Animals)
			{
				if (Animal && Animal->NeedsComponent)
				{
					TotalHappiness += Animal->NeedsComponent->GetNeedValue(FName("Happiness"));
					Count++;
				}
			}
			if (Count > 0)
			{
				AnimalHappinessScore = TotalHappiness / static_cast<float>(Count);
			}
		}
	}

	// --- Visitor Satisfaction (0-1) ---
	VisitorSatisfactionScore = 0.5f;
	if (UVisitorSubsystem* VisitorSub = World->GetSubsystem<UVisitorSubsystem>())
	{
		VisitorSatisfactionScore = VisitorSub->AverageSatisfaction / 100.0f;
	}

	// --- Enclosure Quality (0-1): average condition across all enclosures ---
	EnclosureQualityScore = 0.5f;
	if (UBuildingManagerSubsystem* BuildingMgr = World->GetSubsystem<UBuildingManagerSubsystem>())
	{
		TArray<AEnclosureActor*> Enclosures = BuildingMgr->GetAllEnclosures();
		if (Enclosures.Num() > 0)
		{
			float TotalCondition = 0.0f;
			for (AEnclosureActor* Enc : Enclosures)
			{
				if (Enc)
				{
					TotalCondition += Enc->Condition;
				}
			}
			EnclosureQualityScore = TotalCondition / static_cast<float>(Enclosures.Num());
		}
	}

	// --- Path & Amenities (0-1): based on staff count as proxy ---
	AmenityScore = 0.3f;
	if (UStaffSubsystem* StaffSub = World->GetSubsystem<UStaffSubsystem>())
	{
		// Having 5+ staff members gives full amenity score.
		AmenityScore = FMath::Clamp(static_cast<float>(StaffSub->GetStaffCount()) / 5.0f, 0.0f, 1.0f);
	}

	// --- Weighted total (0-5 stars) ---
	const float OldRating = CurrentRating;
	CurrentRating = (
		AnimalDiversityScore * 0.25f +
		AnimalHappinessScore * 0.25f +
		VisitorSatisfactionScore * 0.20f +
		EnclosureQualityScore * 0.15f +
		AmenityScore * 0.15f
	) * 5.0f;

	CurrentRating = FMath::Clamp(CurrentRating, 0.0f, 5.0f);

	if (!FMath::IsNearlyEqual(OldRating, CurrentRating, 0.05f))
	{
		OnRatingChanged.Broadcast(CurrentRating);
		UE_LOG(LogZooKeeper, Log, TEXT("ZooRatingSubsystem - Rating changed: %.2f -> %.2f stars"), OldRating, CurrentRating);
	}
}

float UZooRatingSubsystem::GetVisitorSpawnMultiplier() const
{
	return 1.0f + CurrentRating * 0.5f;
}

void UZooRatingSubsystem::HandleDayChanged(int32 NewDay)
{
	RecalculateRating();
}
