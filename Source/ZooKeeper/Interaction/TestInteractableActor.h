#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "TestInteractableActor.generated.h"

class UStaticMeshComponent;

/**
 * ATestInteractableActor
 *
 * A minimal interactable actor used for testing and prototyping the
 * interaction system. Renders as a default cube and logs to the output
 * log when interacted with. Drop instances into a level to verify that
 * the UInteractionComponent trace and prompt pipeline works correctly.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Test Interactable Actor"))
class ZOOKEEPER_API ATestInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ATestInteractableActor();

	//~ Begin IInteractable Interface
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	//~ End IInteractable Interface

protected:
	/** Visible mesh for the test actor (defaults to a cube). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Interaction")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
