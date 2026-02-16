#include "ResearchSubsystem.h"
#include "ZooKeeper.h"

bool UResearchSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UResearchSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentResearchID = NAME_None;
	CurrentResearchProgress = 0.0f;
	ResearchDuration = 300.0f; // 300 game seconds per research project
	bIsResearching = false;

	// Populate the master list of research topics.
	// In a production build these would come from a DataTable or config asset.
	AllResearchTopics = {
		FName(TEXT("BetterFeed")),
		FName(TEXT("VeterinaryMedicine")),
		FName(TEXT("EnrichedEnclosures")),
		FName(TEXT("BreedingProgram")),
		FName(TEXT("VisitorAmenities")),
		FName(TEXT("ConservationEfforts")),
		FName(TEXT("AdvancedHabitats")),
		FName(TEXT("NighttimeExhibits")),
		FName(TEXT("EducationalPrograms")),
		FName(TEXT("SustainableEnergy"))
	};

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem::Initialize - %d research topics available."), AllResearchTopics.Num());
}

void UResearchSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem::Deinitialize - %d topics completed."), CompletedResearch.Num());

	CompletedResearch.Empty();
	AllResearchTopics.Empty();

	Super::Deinitialize();
}

void UResearchSubsystem::StartResearch(FName ResearchID)
{
	if (ResearchID.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchSubsystem::StartResearch - Invalid research ID (None)."));
		return;
	}

	if (bIsResearching)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchSubsystem::StartResearch - Research already in progress: '%s'."),
			*CurrentResearchID.ToString());
		return;
	}

	if (CompletedResearch.Contains(ResearchID))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchSubsystem::StartResearch - '%s' is already researched."),
			*ResearchID.ToString());
		return;
	}

	if (!AllResearchTopics.Contains(ResearchID))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchSubsystem::StartResearch - '%s' is not a valid research topic."),
			*ResearchID.ToString());
		return;
	}

	CurrentResearchID = ResearchID;
	CurrentResearchProgress = 0.0f;
	bIsResearching = true;

	OnResearchStarted.Broadcast(ResearchID);

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem - Started researching '%s'."), *ResearchID.ToString());
}

void UResearchSubsystem::TickResearch(float DeltaTime)
{
	if (!bIsResearching)
	{
		return;
	}

	CurrentResearchProgress += DeltaTime;

	if (CurrentResearchProgress >= ResearchDuration)
	{
		// Research complete
		const FName CompletedID = CurrentResearchID;
		CompletedResearch.Add(CompletedID);

		CurrentResearchID = NAME_None;
		CurrentResearchProgress = 0.0f;
		bIsResearching = false;

		OnResearchCompleted.Broadcast(CompletedID);

		UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem - Research completed: '%s'. Total completed: %d"),
			*CompletedID.ToString(), CompletedResearch.Num());
	}
}

void UResearchSubsystem::CancelResearch()
{
	if (!bIsResearching)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem::CancelResearch - No research in progress."));
		return;
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchSubsystem - Cancelled research: '%s' (%.1f%% complete)."),
		*CurrentResearchID.ToString(), (CurrentResearchProgress / ResearchDuration) * 100.0f);

	CurrentResearchID = NAME_None;
	CurrentResearchProgress = 0.0f;
	bIsResearching = false;
}

bool UResearchSubsystem::IsResearched(FName ResearchID) const
{
	return CompletedResearch.Contains(ResearchID);
}

TArray<FName> UResearchSubsystem::GetAvailableResearch() const
{
	TArray<FName> Available;

	for (const FName& Topic : AllResearchTopics)
	{
		if (!CompletedResearch.Contains(Topic) && Topic != CurrentResearchID)
		{
			Available.Add(Topic);
		}
	}

	return Available;
}

FName UResearchSubsystem::GetCurrentResearchID() const
{
	return CurrentResearchID;
}

float UResearchSubsystem::GetCurrentResearchProgress() const
{
	if (!bIsResearching || ResearchDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CurrentResearchProgress / ResearchDuration, 0.0f, 1.0f);
}
