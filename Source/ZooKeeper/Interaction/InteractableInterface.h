#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

/**
 * UInteractable
 *
 * UInterface boilerplate class for the interactable interface.
 * Do not add functions here; use IInteractable instead.
 */
UINTERFACE(MinimalAPI, Blueprintable, meta = (DisplayName = "Interactable"))
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IInteractable
 *
 * Interface for any actor in the zoo that can be interacted with by the player.
 * Implementations include animal enclosures, building doors, information signs,
 * staff management kiosks, and any other interactive world object.
 */
class ZOOKEEPER_API IInteractable
{
	GENERATED_BODY()

public:
	/**
	 * Returns the text prompt displayed to the player when this interactable
	 * is in focus (e.g. "Press E to feed animal", "Press E to open gate").
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Zoo|Interaction")
	FText GetInteractionPrompt() const;

	/**
	 * Called when the player confirms an interaction with this object.
	 * @param Interactor  The actor performing the interaction (typically the player pawn).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Zoo|Interaction")
	void OnInteract(AActor* Interactor);

	/**
	 * Determines whether the given actor is currently allowed to interact
	 * with this object. Implementations may check distance, game state,
	 * cooldown timers, or other prerequisites.
	 * @param Interactor  The actor attempting to interact.
	 * @return true if interaction is permitted.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Zoo|Interaction")
	bool CanInteract(AActor* Interactor) const;
};
