#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AActor;

/** Broadcast when the currently focused interactable actor changes (may be nullptr when focus is lost). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusChanged, AActor*, NewFocus);

/**
 * UInteractionComponent
 *
 * Actor component that handles detecting interactable objects in the world
 * via a camera-aligned line trace each tick. Attach this to the player pawn
 * to enable interaction with any actor implementing IInteractable.
 *
 * Workflow:
 *  1. Every tick, a sphere trace is cast from the owning player's camera.
 *  2. If the trace hits an actor that implements IInteractable and reports
 *     CanInteract() == true, that actor becomes the FocusedActor.
 *  3. UI can bind to OnInteractableFocusChanged to show/hide prompts.
 *  4. Call TryInteract() (e.g. on key press) to execute the interaction.
 */
UCLASS(ClassGroup = (ZooKeeper), meta = (BlueprintSpawnableComponent, DisplayName = "Interaction Component"))
class ZOOKEEPER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	// -------------------------------------------------------------------
	//  Interaction API
	// -------------------------------------------------------------------

	/**
	 * Attempts to interact with the currently focused actor.
	 * Does nothing if there is no focused actor or CanInteract() returns false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Interaction")
	void TryInteract();

	/**
	 * Returns the interaction prompt text for the currently focused actor.
	 * Returns an empty FText if nothing is focused.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Interaction")
	FText GetFocusedInteractionPrompt() const;

	/** Returns true if there is a valid, interactable actor currently in focus. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Interaction")
	bool HasInteractable() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	/** Fired whenever the focused interactable actor changes. Passes nullptr when focus is lost. */
	UPROPERTY(BlueprintAssignable, Category = "Zoo|Interaction")
	FOnInteractableFocusChanged OnInteractableFocusChanged;

	// -------------------------------------------------------------------
	//  Configuration
	// -------------------------------------------------------------------

	/** Maximum distance (in Unreal units) from the camera at which interactables can be detected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Interaction", meta = (ClampMin = "0.0", Units = "cm"))
	float InteractionRange;

	/** Radius of the sphere trace used for detection. A small radius helps snap to objects near the crosshair. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Interaction", meta = (ClampMin = "0.0", Units = "cm"))
	float TraceRadius;

private:
	/** Performs the camera-aligned sphere trace and updates FocusedActor. */
	void PerformInteractionTrace();

	/** Sets the new focused actor and broadcasts the delegate if the focus actually changed. */
	void SetFocusedActor(AActor* NewActor);

	/** Weak pointer to the actor currently under the player's crosshair that implements IInteractable. */
	TWeakObjectPtr<AActor> FocusedActor;
};
