#include "EnclosureVolumeComponent.h"
#include "ZooKeeper/ZooKeeper.h"

UEnclosureVolumeComponent::UEnclosureVolumeComponent()
	: EnclosureHeight(300.0f)
	, CachedArea(0.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnclosureVolumeComponent::SetBoundaryPoints(const TArray<FVector>& InPoints)
{
	BoundaryPoints = InPoints;
	CachedArea = CalculateArea();

	OnEnclosureBoundsChanged.Broadcast();

	UE_LOG(LogZooKeeper, Verbose, TEXT("EnclosureVolume: Boundary updated with %d points, area = %.1f"),
		BoundaryPoints.Num(), CachedArea);
}

float UEnclosureVolumeComponent::CalculateArea() const
{
	const int32 NumPoints = BoundaryPoints.Num();
	if (NumPoints < 3)
	{
		return 0.0f;
	}

	// Shoelace formula for 2D polygon area (using X and Y)
	float Area = 0.0f;
	for (int32 i = 0; i < NumPoints; ++i)
	{
		const FVector& Current = BoundaryPoints[i];
		const FVector& Next = BoundaryPoints[(i + 1) % NumPoints];
		Area += (Current.X * Next.Y) - (Next.X * Current.Y);
	}

	return FMath::Abs(Area) * 0.5f;
}

bool UEnclosureVolumeComponent::IsPointInside(FVector Point) const
{
	const int32 NumPoints = BoundaryPoints.Num();
	if (NumPoints < 3)
	{
		return false;
	}

	// Transform the test point into local space
	const FTransform ComponentTransform = GetComponentTransform();
	const FVector LocalPoint = ComponentTransform.InverseTransformPosition(Point);

	// Ray casting algorithm for 2D point-in-polygon (ignoring Z)
	bool bInside = false;
	for (int32 i = 0, j = NumPoints - 1; i < NumPoints; j = i++)
	{
		const FVector& PointI = BoundaryPoints[i];
		const FVector& PointJ = BoundaryPoints[j];

		if (((PointI.Y > LocalPoint.Y) != (PointJ.Y > LocalPoint.Y)) &&
			(LocalPoint.X < (PointJ.X - PointI.X) * (LocalPoint.Y - PointI.Y) / (PointJ.Y - PointI.Y) + PointI.X))
		{
			bInside = !bInside;
		}
	}

	return bInside;
}

FVector UEnclosureVolumeComponent::GetRandomPointInside() const
{
	const FBox Bounds = GetBoundingBox();

	if (!Bounds.IsValid || BoundaryPoints.Num() < 3)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("EnclosureVolume: Cannot get random point - insufficient boundary points."));
		return GetComponentLocation();
	}

	// Rejection sampling: pick random points in the bounding box until one is inside the polygon
	for (int32 Attempt = 0; Attempt < MaxRejectionSamples; ++Attempt)
	{
		const FVector RandomPoint = FMath::RandPointInBox(Bounds);

		if (IsPointInside(RandomPoint))
		{
			return RandomPoint;
		}
	}

	// Fallback: return the component location if rejection sampling failed
	UE_LOG(LogZooKeeper, Warning, TEXT("EnclosureVolume: Rejection sampling exhausted after %d attempts, returning component location."),
		MaxRejectionSamples);
	return GetComponentLocation();
}

bool UEnclosureVolumeComponent::IsClosedPolygon() const
{
	if (BoundaryPoints.Num() < 3)
	{
		return false;
	}

	const float Distance = FVector::Dist(BoundaryPoints[0], BoundaryPoints.Last());
	return Distance <= ClosedPolygonThreshold;
}

FBox UEnclosureVolumeComponent::GetBoundingBox() const
{
	if (BoundaryPoints.Num() == 0)
	{
		return FBox(ForceInit);
	}

	const FTransform ComponentTransform = GetComponentTransform();

	FBox Bounds(ForceInit);
	for (const FVector& LocalPoint : BoundaryPoints)
	{
		const FVector WorldPoint = ComponentTransform.TransformPosition(LocalPoint);
		Bounds += WorldPoint;
	}

	return Bounds;
}
