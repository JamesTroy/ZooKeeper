#include "EnrichmentItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "ZooKeeper/ZooKeeper.h"

AEnrichmentItemActor::AEnrichmentItemActor()
	: HappinessBoost(0.1f)
	, Cost(200)
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

FText AEnrichmentItemActor::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(FString::Printf(TEXT("Enrichment: %s (+%.0f%% Happiness)"),
		*DisplayName.ToString(), HappinessBoost * 100.0f));
}

void AEnrichmentItemActor::OnInteract_Implementation(AActor* Interactor)
{
	UE_LOG(LogZooKeeper, Log, TEXT("EnrichmentItemActor: Interacted with '%s'."), *ItemID.ToString());
}

bool AEnrichmentItemActor::IsCompatibleWithSpecies(FName SpeciesID) const
{
	// Empty list means compatible with all species.
	if (CompatibleSpecies.Num() == 0)
	{
		return true;
	}

	return CompatibleSpecies.Contains(SpeciesID);
}
