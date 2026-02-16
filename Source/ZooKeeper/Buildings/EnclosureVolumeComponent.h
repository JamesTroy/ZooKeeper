#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnclosureVolumeComponent.generated.h"

/** Broadcast when the enclosure boundary points are modified. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnclosureBoundsChanged);

/**
 * UEnclosureVolumeComponent
 *
 * Defines the walkable volume of an animal enclosure as a 2D polygon
 * extruded to a configurable height. Provides spatial queries such as
 * point-in-polygon testing and random point generation for AI movement.
 */
UCLASS(ClassGroup = (Zoo), meta = (BlueprintSpawnableComponent, DisplayName = "Enclosure Volume"))
class ZOOKEEPER_API UEnclosureVolumeComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UEnclosureVolumeComponent();

	// -------------------------------------------------------------------
	//  Configuration
	// -------------------------------------------------------------------

	/** The fence post positions forming the enclosure polygon (in local space). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure|Volume")
	TArray<FVector> BoundaryPoints;

	/** The height of the enclosure walls. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Enclosure|Volume", meta = (ClampMin = "0.0"))
	float EnclosureHeight;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/**
	 * Sets the boundary points and recalculates the cached area.
	 * @param InPoints  The new boundary polygon vertices.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Enclosure|Volume")
	void SetBoundaryPoints(const TArray<FVector>& InPoints);

	/**
	 * Calculates the 2D area of the polygon using the Shoelace formula.
	 * @return The area in square unreal units.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Volume")
	float CalculateArea() const;

	/**
	 * Tests whether a world-space point lies inside the enclosure polygon (2D test, ignores Z).
	 * @param Point  The world-space point to test.
	 * @return true if the point is inside the polygon.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Volume")
	bool IsPointInside(FVector Point) const;

	/**
	 * Returns a random world-space point inside the enclosure polygon
	 * using rejection sampling within the bounding box.
	 * @return A random valid point inside the enclosure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Enclosure|Volume")
	FVector GetRandomPointInside() const;

	/**
	 * Checks whether the polygon is closed (first point is near the last point).
	 * @return true if the polygon forms a closed loop.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Volume")
	bool IsClosedPolygon() const;

	/**
	 * Returns the axis-aligned bounding box of the boundary polygon in world space.
	 * @return The bounding box.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Enclosure|Volume")
	FBox GetBoundingBox() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Enclosure|Volume")
	FOnEnclosureBoundsChanged OnEnclosureBoundsChanged;

private:
	/** Cached area value, updated when boundary points change. */
	float CachedArea;

	/** Threshold distance for considering the polygon closed. */
	static constexpr float ClosedPolygonThreshold = 10.0f;

	/** Maximum iterations for rejection sampling in GetRandomPointInside. */
	static constexpr int32 MaxRejectionSamples = 1000;
};
