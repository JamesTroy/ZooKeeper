#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AnimalSpeciesRow.generated.h"

/**
 * FAnimalSpeciesRow
 *
 * Data-table row that defines the static properties of an animal species.
 * Rows are keyed by a unique FName (e.g. "Lion", "Elephant", "Penguin").
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FAnimalSpeciesRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Human-readable display name (e.g. "African Lion"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species")
	FText DisplayName;

	/** Short description shown on information plaques. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species")
	FText Description;

	/** Maximum age in game-days before the animal dies of old age. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species", meta = (ClampMin = "1"))
	int32 MaxAge = 100;

	/** Base purchase price for adopting this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species", meta = (ClampMin = "0"))
	int32 BaseCost = 1000;

	/** Minimum enclosure area (square metres) required. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species", meta = (ClampMin = "0.0"))
	float MinEnclosureArea = 50.0f;

	/** Preferred biome tag (e.g. "Savannah", "Arctic", "Tropical"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species")
	FName PreferredBiome;

	/** Skeletal mesh to use for this species. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	/** Behavior tree asset to drive the animal AI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Species")
	TSoftObjectPtr<UObject> BehaviorTree;
};
