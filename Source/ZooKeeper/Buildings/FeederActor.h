#pragma once

#include "CoreMinimal.h"
#include "ZooBuildingActor.h"
#include "Data/ZooDataTypes.h"
#include "FeederActor.generated.h"

/** Broadcast when the feeder runs out of food. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoodDepleted, AFeederActor*, Feeder);

/** Broadcast when the feeder is restocked. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFeederRestocked, AFeederActor*, Feeder, int32, NewStock);

/**
 * AFeederActor
 *
 * Placeable feeder that holds food for animals. Animals consume food from the
 * feeder through their BTTask_AnimalEat behavior. Players can restock the
 * feeder via the interaction system (costs money).
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Feeder"))
class ZOOKEEPER_API AFeederActor : public AZooBuildingActor
{
	GENERATED_BODY()

public:
	AFeederActor();

	// -------------------------------------------------------------------
	//  IInteractable Implementation
	// -------------------------------------------------------------------

	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void  OnInteract_Implementation(AActor* Interactor) override;
	virtual bool  CanInteract_Implementation(AActor* Interactor) const override;

	// -------------------------------------------------------------------
	//  Properties
	// -------------------------------------------------------------------

	/** The type of food this feeder dispenses. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Feeder")
	EFoodType FoodType;

	/** Maximum number of food units this feeder can hold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Feeder", meta = (ClampMin = "1"))
	int32 MaxCapacity;

	/** Current number of food units in the feeder. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Feeder")
	int32 CurrentStock;

	/** Cost per unit to restock. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Feeder", meta = (ClampMin = "0"))
	int32 RestockCostPerUnit;

	/** How much hunger is restored per food unit consumed (0-1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Feeder", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HungerRestorePerUse;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/**
	 * Restocks the feeder by the given amount (clamped to MaxCapacity).
	 * @param Amount  Number of units to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Feeder")
	void Restock(int32 Amount);

	/**
	 * An animal consumes one unit of food from this feeder.
	 * @return true if food was available and consumed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Feeder")
	bool ConsumeFood();

	/** Returns true if the feeder has no food left. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Feeder")
	bool IsEmpty() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Feeder")
	FOnFoodDepleted OnFoodDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Feeder")
	FOnFeederRestocked OnFeederRestocked;
};
