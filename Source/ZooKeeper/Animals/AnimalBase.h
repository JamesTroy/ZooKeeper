#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractableInterface.h"
#include "AnimalBase.generated.h"

class UAnimalNeedsComponent;
class AEnclosureActor;
struct FAnimalSpeciesRow;
class UDataTable;

/** Broadcast when the player inspects this animal. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalInspected, AAnimalBase*, Animal);

/** Broadcast when any of this animal's needs enters the critical range. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimalNeedCritical, AAnimalBase*, Animal, FName, NeedName);

/**
 * AAnimalBase
 *
 * Base class for all animals in the zoo. Provides a needs component, species
 * data-table lookup, interaction support, and automatic registration with the
 * animal manager subsystem.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Animal Base"))
class ZOOKEEPER_API AAnimalBase : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	AAnimalBase();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	// -------------------------------------------------------------------
	//  IInteractable Implementation
	// -------------------------------------------------------------------

	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void  OnInteract_Implementation(AActor* Interactor) override;
	virtual bool  CanInteract_Implementation(AActor* Interactor) const override;

	// -------------------------------------------------------------------
	//  Species & Identity
	// -------------------------------------------------------------------

	/** Row name used to look up this species in the animal data table. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal")
	FName SpeciesID;

	/** Display name for this individual animal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal")
	FString AnimalName;

	/** Age of the animal in game-days. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal")
	int32 Age;

	/** Data table containing species definitions (FAnimalSpeciesRow). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Animal")
	UDataTable* SpeciesDataTable;

	/** Walking speed in cm/s, loaded from species DataTable on BeginPlay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Animal|Movement")
	float AnimalWalkSpeed;

	/** Running speed in cm/s, loaded from species DataTable on BeginPlay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Animal|Movement")
	float AnimalRunSpeed;

	/**
	 * Looks up the species row in SpeciesDataTable using SpeciesID.
	 * @return Pointer to the row, or nullptr if not found.
	 */
	const FAnimalSpeciesRow* GetSpeciesData() const;

	// -------------------------------------------------------------------
	//  Enclosure
	// -------------------------------------------------------------------

	/** The enclosure this animal currently resides in. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zoo|Animal")
	AEnclosureActor* CurrentEnclosure;

	// -------------------------------------------------------------------
	//  Components
	// -------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Animal")
	UAnimalNeedsComponent* NeedsComponent;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animal")
	FOnAnimalInspected OnAnimalInspected;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Animal")
	FOnAnimalNeedCritical OnAnimalNeedCritical;

private:
	/** Callback bound to the needs component's OnNeedCritical delegate. */
	UFUNCTION()
	void HandleNeedCritical(FName NeedName);
};
