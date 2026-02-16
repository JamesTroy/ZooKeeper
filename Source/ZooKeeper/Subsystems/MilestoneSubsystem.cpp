#include "MilestoneSubsystem.h"
#include "AnimalManagerSubsystem.h"
#include "VisitorSubsystem.h"
#include "EconomySubsystem.h"
#include "BuildingManagerSubsystem.h"
#include "ZooRatingSubsystem.h"
#include "TimeSubsystem.h"
#include "Animals/AnimalBase.h"
#include "ZooKeeper.h"

bool UMilestoneSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UMilestoneSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->OnDayChanged.AddDynamic(this, &UMilestoneSubsystem::HandleDayChanged);
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("MilestoneSubsystem::Initialize"));
}

void UMilestoneSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("MilestoneSubsystem::Deinitialize - %d milestones achieved."), AchievedMilestones.Num());
	Super::Deinitialize();
}

void UMilestoneSubsystem::CheckMilestones()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// --- FirstSteps: Have at least 1 animal and 1 enclosure ---
	if (!AchievedMilestones.Contains(FName("FirstSteps")))
	{
		UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>();
		UBuildingManagerSubsystem* BuildingMgr = World->GetSubsystem<UBuildingManagerSubsystem>();
		if (AnimalMgr && AnimalMgr->GetAnimalCount() > 0 &&
			BuildingMgr && BuildingMgr->GetAllEnclosures().Num() > 0)
		{
			AwardMilestone(FName("FirstSteps"));
		}
	}

	// --- GrowingZoo: 5+ animals of 3+ species ---
	if (!AchievedMilestones.Contains(FName("GrowingZoo")))
	{
		if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
		{
			TArray<AAnimalBase*> Animals = AnimalMgr->GetAnimalsInEnclosure(nullptr);
			if (Animals.Num() >= 5)
			{
				TSet<FName> Species;
				for (AAnimalBase* Animal : Animals)
				{
					if (Animal)
					{
						Species.Add(Animal->SpeciesID);
					}
				}
				if (Species.Num() >= 3)
				{
					AwardMilestone(FName("GrowingZoo"));
				}
			}
		}
	}

	// --- Popular: 20+ visitors ---
	if (!AchievedMilestones.Contains(FName("Popular")))
	{
		if (UVisitorSubsystem* VisitorSub = World->GetSubsystem<UVisitorSubsystem>())
		{
			if (VisitorSub->CurrentVisitorCount >= 20)
			{
				AwardMilestone(FName("Popular"));
			}
		}
	}

	// --- Paradise: 4.0+ zoo rating ---
	if (!AchievedMilestones.Contains(FName("Paradise")))
	{
		if (UZooRatingSubsystem* RatingSub = World->GetSubsystem<UZooRatingSubsystem>())
		{
			if (RatingSub->GetRating() >= 4.0f)
			{
				AwardMilestone(FName("Paradise"));
			}
		}
	}

	// --- FiveStars: 5.0 zoo rating ---
	if (!AchievedMilestones.Contains(FName("FiveStars")))
	{
		if (UZooRatingSubsystem* RatingSub = World->GetSubsystem<UZooRatingSubsystem>())
		{
			if (RatingSub->GetRating() >= 4.95f)
			{
				AwardMilestone(FName("FiveStars"));
			}
		}
	}

	// --- Tycoon: $50,000+ funds ---
	if (!AchievedMilestones.Contains(FName("Tycoon")))
	{
		if (UEconomySubsystem* EconSub = World->GetSubsystem<UEconomySubsystem>())
		{
			if (EconSub->GetBalance() >= 50000)
			{
				AwardMilestone(FName("Tycoon"));
			}
		}
	}
}

bool UMilestoneSubsystem::IsMilestoneAchieved(FName MilestoneID) const
{
	return AchievedMilestones.Contains(MilestoneID);
}

TArray<FName> UMilestoneSubsystem::GetAchievedMilestones() const
{
	return AchievedMilestones.Array();
}

void UMilestoneSubsystem::HandleDayChanged(int32 NewDay)
{
	CheckMilestones();
}

void UMilestoneSubsystem::AwardMilestone(FName MilestoneID)
{
	if (AchievedMilestones.Contains(MilestoneID))
	{
		return;
	}

	AchievedMilestones.Add(MilestoneID);
	OnMilestoneAchieved.Broadcast(MilestoneID);

	UE_LOG(LogZooKeeper, Log, TEXT("MilestoneSubsystem - Milestone achieved: '%s'! Total: %d"),
		*MilestoneID.ToString(), AchievedMilestones.Num());
}
