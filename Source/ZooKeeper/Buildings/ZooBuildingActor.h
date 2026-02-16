#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZooKeeper/Interaction/InteractableInterface.h"
#include "ZooKeeper/SaveLoad/ZooSaveable.h"
#include "ZooBuildingActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UBuildingManagerSubsystem;

/** Broadcast when a player interacts with this building. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuildingInteracted, AZooBuildingActor*, Building, AActor*, Interactor);

/** Broadcast when the building's condition value changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConditionChanged, AZooBuildingActor*, Building, float, NewCondition);

/**
 * AZooBuildingActor
 *
 * Base class for all placeable buildings in the zoo. Handles mesh display,
 * collision, condition degradation, interaction prompts, and save/load
 * serialization. Concrete building types (enclosures, facilities, etc.)
 * should inherit from this class.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Zoo Building"))
class ZOOKEEPER_API AZooBuildingActor : public AActor, public IInteractable, public IZooSaveable
{
	GENERATED_BODY()

public:
	AZooBuildingActor();

	// -------------------------------------------------------------------
	//  Components
	// -------------------------------------------------------------------

	/** The primary visual mesh for this building. Also serves as the root component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Components")
	TObjectPtr<UStaticMeshComponent> BuildingMesh;

	/** Box collision used for overlap and placement validation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	// -------------------------------------------------------------------
	//  Properties
	// -------------------------------------------------------------------

	/** Unique identifier for this building type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Identity")
	FName BuildingID;

	/** Human-readable name displayed in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Identity")
	FString BuildingName;

	/** Current condition of the building, 0.0 (ruined) to 1.0 (pristine). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|State", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Condition;

	/** One-time cost to place this building. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Economy", meta = (ClampMin = "0"))
	int32 PurchaseCost;

	/** Daily maintenance cost in zoo currency. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Building|Economy", meta = (ClampMin = "0.0"))
	float MaintenanceCostPerDay;

	/** Whether this building has been placed in the world (vs. being a ghost preview). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|State")
	bool bIsPlaced;

	/** Persistent unique ID used by the save system. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Building|SaveLoad")
	FGuid SaveID;

	// -------------------------------------------------------------------
	//  IInteractable Interface
	// -------------------------------------------------------------------

	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;

	// -------------------------------------------------------------------
	//  IZooSaveable Interface
	// -------------------------------------------------------------------

	virtual void SerializeSaveData_Implementation(FArchive& Ar) override;
	virtual FGuid GetSaveID_Implementation() const override;

	// -------------------------------------------------------------------
	//  Building Functions
	// -------------------------------------------------------------------

	/**
	 * Reduces the building's condition by the specified amount.
	 * @param Amount  The amount to degrade (clamped so Condition stays >= 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building")
	void DegradeCondition(float Amount);

	/**
	 * Fully repairs the building, restoring Condition to 1.0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Building")
	void Repair();

	/**
	 * Returns the daily maintenance cost as an integer (for economy system integration).
	 * @return Maintenance cost rounded to the nearest integer.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Building|Economy")
	int32 GetMaintenanceCost() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Building|Events")
	FOnBuildingInteracted OnBuildingInteracted;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Building|Events")
	FOnConditionChanged OnConditionChanged;

protected:
	virtual void BeginPlay() override;
};
