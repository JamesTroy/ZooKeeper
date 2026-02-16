#include "BuildingPlacementComponent.h"
#include "ZooBuildingActor.h"
#include "ZooKeeper/ZooKeeper.h"
#include "ZooKeeper/Subsystems/EconomySubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionQueryParams.h"

UBuildingPlacementComponent::UBuildingPlacementComponent()
	: bIsInBuildMode(false)
	, GridSize(100.0f)
	, RotationStep(90.0f)
	, CurrentPlacementYaw(0.0f)
	, bLastPlacementValid(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBuildingPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInBuildMode)
	{
		UpdatePlacement();
	}
}

// -------------------------------------------------------------------
//  Build Mode Functions
// -------------------------------------------------------------------

void UBuildingPlacementComponent::EnterBuildMode()
{
	if (bIsInBuildMode)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Already in build mode."));
		return;
	}

	if (!SelectedBuildingClass)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Cannot enter build mode without a selected building class."));
		return;
	}

	bIsInBuildMode = true;
	CurrentPlacementYaw = 0.0f;
	bLastPlacementValid = false;

	SpawnGhostPreview();

	// Enable ticking for placement updates
	PrimaryComponentTick.SetTickFunctionEnable(true);

	OnBuildModeChanged.Broadcast(true);
	UE_LOG(LogZooKeeper, Log, TEXT("BuildingPlacement: Entered build mode with class '%s'."),
		*SelectedBuildingClass->GetName());
}

void UBuildingPlacementComponent::ExitBuildMode()
{
	if (!bIsInBuildMode)
	{
		return;
	}

	bIsInBuildMode = false;
	DestroyGhostPreview();

	// Disable ticking when not in build mode
	PrimaryComponentTick.SetTickFunctionEnable(false);

	OnBuildModeChanged.Broadcast(false);
	UE_LOG(LogZooKeeper, Log, TEXT("BuildingPlacement: Exited build mode."));
}

void UBuildingPlacementComponent::SelectBuilding(TSubclassOf<AZooBuildingActor> NewBuildingClass)
{
	if (!NewBuildingClass)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Attempted to select null building class."));
		return;
	}

	SelectedBuildingClass = NewBuildingClass;
	CurrentPlacementYaw = 0.0f;

	// If already in build mode, respawn the ghost with the new class
	if (bIsInBuildMode)
	{
		DestroyGhostPreview();
		SpawnGhostPreview();
	}

	UE_LOG(LogZooKeeper, Log, TEXT("BuildingPlacement: Selected building class '%s'."),
		*NewBuildingClass->GetName());
}

void UBuildingPlacementComponent::UpdatePlacement()
{
	if (!GhostPreviewActor || !bIsInBuildMode)
	{
		return;
	}

	// Get the player controller that owns this component
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return;
	}

	// Get the camera location and direction for the ground trace
	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * MaxTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GhostPreviewActor);
	QueryParams.AddIgnoredActor(PC->GetPawn());

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		GroundTraceChannel,
		QueryParams
	);

	if (bHit)
	{
		// Snap the hit location to the grid
		FVector SnappedLocation = SnapToGrid(HitResult.Location);

		// Apply current rotation
		FRotator PlacementRotation = FRotator(0.0f, CurrentPlacementYaw, 0.0f);

		GhostPreviewActor->SetActorLocationAndRotation(SnappedLocation, PlacementRotation);

		// Update validity and ghost material
		const bool bValid = IsPlacementValid();
		if (bValid != bLastPlacementValid)
		{
			UpdateGhostMaterial(bValid);
			bLastPlacementValid = bValid;
		}
	}
}

void UBuildingPlacementComponent::RotatePlacement(float Direction)
{
	if (!bIsInBuildMode || !GhostPreviewActor)
	{
		return;
	}

	const float RotationDelta = (Direction >= 0.0f) ? RotationStep : -RotationStep;
	CurrentPlacementYaw = FMath::Fmod(CurrentPlacementYaw + RotationDelta, 360.0f);

	UE_LOG(LogZooKeeper, Verbose, TEXT("BuildingPlacement: Rotated to %.1f degrees."), CurrentPlacementYaw);
}

bool UBuildingPlacementComponent::ConfirmPlacement()
{
	if (!bIsInBuildMode || !GhostPreviewActor || !SelectedBuildingClass)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Cannot confirm - not in build mode or no selection."));
		return false;
	}

	if (!IsPlacementValid())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Cannot confirm - placement is invalid."));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	// Deduct cost via EconomySubsystem
	if (AZooBuildingActor* GhostBuilding = Cast<AZooBuildingActor>(GhostPreviewActor))
	{
		const int32 BuildCost = GhostBuilding->PurchaseCost;
		if (BuildCost > 0)
		{
			if (UEconomySubsystem* Economy = World->GetSubsystem<UEconomySubsystem>())
			{
				if (!Economy->TrySpend(BuildCost, FString::Printf(TEXT("Building: %s"), *GhostBuilding->BuildingName)))
				{
					UE_LOG(LogZooKeeper, Warning, TEXT("BuildingPlacement: Cannot afford building '%s' (cost: %d)."),
						*GhostBuilding->BuildingName, BuildCost);
					return false;
				}
			}
		}
	}

	// Spawn the real building actor at the ghost's location and rotation
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector SpawnLocation = GhostPreviewActor->GetActorLocation();
	const FRotator SpawnRotation = GhostPreviewActor->GetActorRotation();

	AZooBuildingActor* NewBuilding = World->SpawnActor<AZooBuildingActor>(
		SelectedBuildingClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!NewBuilding)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("BuildingPlacement: Failed to spawn building actor."));
		return false;
	}

	NewBuilding->bIsPlaced = true;

	OnBuildingPlaced.Broadcast(NewBuilding);
	UE_LOG(LogZooKeeper, Log, TEXT("BuildingPlacement: Placed building '%s' at %s."),
		*NewBuilding->BuildingName, *SpawnLocation.ToString());

	// Exit build mode after successful placement
	ExitBuildMode();

	return true;
}

void UBuildingPlacementComponent::CancelPlacement()
{
	UE_LOG(LogZooKeeper, Log, TEXT("BuildingPlacement: Placement cancelled."));
	ExitBuildMode();
}

bool UBuildingPlacementComponent::IsPlacementValid() const
{
	if (!GhostPreviewActor || !bIsInBuildMode)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	// Check 1: Overlap test - ensure the ghost is not overlapping with existing buildings
	TArray<AActor*> OverlappingActors;
	GhostPreviewActor->GetOverlappingActors(OverlappingActors, AZooBuildingActor::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		return false;
	}

	// Check 2: Verify the ghost is on valid terrain (not floating in the air)
	// A simple downward trace from the ghost's position should hit the ground
	const FVector GhostLocation = GhostPreviewActor->GetActorLocation();
	FHitResult GroundHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GhostPreviewActor);

	const bool bOnGround = World->LineTraceSingleByChannel(
		GroundHit,
		GhostLocation + FVector(0.0f, 0.0f, 50.0f),
		GhostLocation - FVector(0.0f, 0.0f, 200.0f),
		GroundTraceChannel,
		QueryParams
	);

	if (!bOnGround)
	{
		return false;
	}

	// Check 3: Verify the player can afford the building
	if (AZooBuildingActor* GhostBuilding = Cast<AZooBuildingActor>(GhostPreviewActor))
	{
		const int32 BuildCost = GhostBuilding->PurchaseCost;
		if (BuildCost > 0)
		{
			if (UEconomySubsystem* Economy = World->GetSubsystem<UEconomySubsystem>())
			{
				if (Economy->GetBalance() < BuildCost)
				{
					return false;
				}
			}
		}
	}

	return true;
}

// -------------------------------------------------------------------
//  Private Helpers
// -------------------------------------------------------------------

FVector UBuildingPlacementComponent::SnapToGrid(FVector InLocation) const
{
	if (GridSize <= 0.0f)
	{
		return InLocation;
	}

	FVector Snapped;
	Snapped.X = FMath::RoundToFloat(InLocation.X / GridSize) * GridSize;
	Snapped.Y = FMath::RoundToFloat(InLocation.Y / GridSize) * GridSize;
	Snapped.Z = InLocation.Z; // Keep original Z (ground height)

	return Snapped;
}

void UBuildingPlacementComponent::UpdateGhostMaterial(bool bValid)
{
	if (!GhostPreviewActor)
	{
		return;
	}

	UMaterialInterface* MaterialToApply = bValid ? ValidPlacementMaterial.Get() : InvalidPlacementMaterial.Get();
	if (!MaterialToApply)
	{
		return;
	}

	// Apply the material to all static mesh components on the ghost preview
	TArray<UStaticMeshComponent*> MeshComponents;
	GhostPreviewActor->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (MeshComp)
		{
			const int32 NumMaterials = MeshComp->GetNumMaterials();
			for (int32 i = 0; i < NumMaterials; ++i)
			{
				MeshComp->SetMaterial(i, MaterialToApply);
			}
		}
	}
}

void UBuildingPlacementComponent::DestroyGhostPreview()
{
	if (GhostPreviewActor)
	{
		GhostPreviewActor->Destroy();
		GhostPreviewActor = nullptr;
	}
}

void UBuildingPlacementComponent::SpawnGhostPreview()
{
	DestroyGhostPreview();

	UWorld* World = GetWorld();
	if (!World || !SelectedBuildingClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GhostPreviewActor = World->SpawnActor<AZooBuildingActor>(
		SelectedBuildingClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!GhostPreviewActor)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("BuildingPlacement: Failed to spawn ghost preview actor."));
		return;
	}

	// Disable collision on the ghost so it doesn't block placement
	GhostPreviewActor->SetActorEnableCollision(false);

	// Mark the ghost as not placed (it's just a preview)
	if (AZooBuildingActor* GhostBuilding = Cast<AZooBuildingActor>(GhostPreviewActor))
	{
		GhostBuilding->bIsPlaced = false;
	}

	// Apply the initial ghost material
	UpdateGhostMaterial(false);

	UE_LOG(LogZooKeeper, Verbose, TEXT("BuildingPlacement: Ghost preview spawned."));
}
