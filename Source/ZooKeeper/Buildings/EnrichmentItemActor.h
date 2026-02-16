#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableInterface.h"
#include "EnrichmentItemActor.generated.h"

/**
 * AEnrichmentItemActor
 *
 * Placeable enrichment item (climbing frame, tire swing, pool, etc.)
 * that boosts animal happiness when placed inside an enclosure.
 * Reads configuration from DT_EnrichmentItems.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Enrichment Item Actor"))
class ZOOKEEPER_API AEnrichmentItemActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AEnrichmentItemActor();

	// IInteractable
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	/** The enrichment item identifier (matches row name in DT_EnrichmentItems). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	FName ItemID;

	/** Display name for the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	FText DisplayName;

	/** How much happiness this item adds per tick to nearby animals. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment", meta = (ClampMin = "0.0"))
	float HappinessBoost;

	/** Purchase cost. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment", meta = (ClampMin = "0"))
	int32 Cost;

	/** Species that benefit from this enrichment (empty = all species). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enrichment")
	TArray<FName> CompatibleSpecies;

	/** Returns true if the given species benefits from this item. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enrichment")
	bool IsCompatibleWithSpecies(FName SpeciesID) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Enrichment")
	TObjectPtr<UStaticMeshComponent> MeshComp;
};
