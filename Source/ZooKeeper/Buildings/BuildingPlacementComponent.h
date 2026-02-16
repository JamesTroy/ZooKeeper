#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingPlacementComponent.generated.h"

class AZooBuildingActor;
class UMaterialInterface;

/** Broadcast when placement build mode is entered or exited. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlacementBuildModeChanged, bool, bIsInBuildMode);

/** Broadcast when a building placement is confirmed. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingPlacementConfirmed, AZooBuildingActor*, PlacedBuilding);

/**
 * UBuildingPlacementComponent
 *
 * Actor component (intended for the PlayerController) that manages the
 * building placement workflow: entering build mode, showing a translucent
 * ghost preview, snapping to a grid, validating placement, and spawning
 * the final building actor.
 */
UCLASS(ClassGroup = (Zoo), meta = (BlueprintSpawnableComponent, DisplayName = "Building Placement"))
class ZOOKEEPER_API UBuildingPlacementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildingPlacementComponent();

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	// -------------------------------------------------------------------
	//  Configuration
	// -------------------------------------------------------------------

	/** Whether the player is currently in build mode. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Placement")
	bool bIsInBuildMode;

	/** The building class currently selected for placement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Placement")
	TSubclassOf<AZooBuildingActor> SelectedBuildingClass;

	/** Grid cell size for snapping placement positions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Placement", meta = (ClampMin = "1.0"))
	float GridSize;

	/** Degrees to rotate the ghost preview per rotation step. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Placement", meta = (ClampMin = "1.0", ClampMax = "360.0"))
	float RotationStep;

	// -------------------------------------------------------------------
	//  Preview Ghost
	// -------------------------------------------------------------------

	/** The translucent preview actor shown during placement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Placement")
	TObjectPtr<AActor> GhostPreviewActor;

	/** Material applied to the ghost when placement is valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Placement|Materials")
	TObjectPtr<UMaterialInterface> ValidPlacementMaterial;

	/** Material applied to the ghost when placement is invalid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Placement|Materials")
	TObjectPtr<UMaterialInterface> InvalidPlacementMaterial;

	// -------------------------------------------------------------------
	//  Build Mode Functions
	// -------------------------------------------------------------------

	/**
	 * Enters build mode, spawning a ghost preview of the selected building.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void EnterBuildMode();

	/**
	 * Exits build mode and destroys the ghost preview actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void ExitBuildMode();

	/**
	 * Changes the selected building class and respawns the ghost preview.
	 * @param NewBuildingClass  The new building class to select.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void SelectBuilding(TSubclassOf<AZooBuildingActor> NewBuildingClass);

	/**
	 * Updates the ghost preview position and validity each frame.
	 * Ray traces from the camera to the ground, snaps to grid, and
	 * updates the ghost material based on placement validity.
	 * Called automatically during TickComponent when in build mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void UpdatePlacement();

	/**
	 * Rotates the ghost preview by one RotationStep in the specified direction.
	 * @param Direction  Positive for clockwise, negative for counter-clockwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void RotatePlacement(float Direction);

	/**
	 * Attempts to finalize the placement. If valid, spawns the real building
	 * actor and deducts the cost via the EconomySubsystem.
	 * @return true if the building was successfully placed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	bool ConfirmPlacement();

	/**
	 * Cancels the current placement and exits build mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building|Placement")
	void CancelPlacement();

	/**
	 * Checks whether the current ghost position is a valid placement location.
	 * Validates against overlaps, terrain, and available funds.
	 * @return true if placement is valid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Building|Placement")
	bool IsPlacementValid() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Building|Placement|Events")
	FOnPlacementBuildModeChanged OnPlacementBuildModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Building|Placement|Events")
	FOnBuildingPlacementConfirmed OnBuildingPlacementConfirmed;

private:
	/**
	 * Snaps a world position to the placement grid.
	 * @param InLocation  The raw world position.
	 * @return The grid-snapped position.
	 */
	FVector SnapToGrid(FVector InLocation) const;

	/**
	 * Updates the ghost preview's material to reflect validity.
	 * @param bValid  true to apply ValidPlacementMaterial, false for InvalidPlacementMaterial.
	 */
	void UpdateGhostMaterial(bool bValid);

	/** Destroys the ghost preview actor if it exists. */
	void DestroyGhostPreview();

	/** Spawns the ghost preview actor from the selected building class. */
	void SpawnGhostPreview();

	/** The current rotation applied to the ghost preview. */
	float CurrentPlacementYaw;

	/** Cached result of the last validity check. */
	bool bLastPlacementValid;

	/** Trace channel used for ground detection. */
	static constexpr ECollisionChannel GroundTraceChannel = ECC_Visibility;

	/** Maximum trace distance for ground detection. */
	static constexpr float MaxTraceDistance = 50000.0f;
};
