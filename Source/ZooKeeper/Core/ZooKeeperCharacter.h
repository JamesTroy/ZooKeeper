#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZooKeeperCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInteractionComponent;
struct FInputActionValue;

/**
 * AZooKeeperCharacter
 *
 * The player-controlled zoo keeper character. Features first-person camera,
 * Enhanced Input-driven movement, sprint toggle, and an interaction component
 * for engaging with animals, buildings, and objects in the zoo.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Keeper Character"))
class ZOOKEEPER_API AZooKeeperCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AZooKeeperCharacter();

	//~ Begin APawn Interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End APawn Interface

	//~ Begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	/** Returns the first-person camera component. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Camera")
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	/** Returns the interaction component. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Interaction")
	UInteractionComponent* GetInteractionComponent() const { return InteractionComp; }

	/** Returns whether the character is currently sprinting. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Movement")
	bool IsSprinting() const { return bIsSprinting; }

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	// ---------------------------------------------------------------
	//  Components
	// ---------------------------------------------------------------

	/** First-person camera attached to the capsule component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/** Handles interaction traces and prompts for nearby interactable objects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComp;

	// ---------------------------------------------------------------
	//  Enhanced Input Actions (assigned in Blueprint)
	// ---------------------------------------------------------------

	/** Input action for movement (2D axis: forward/right). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Move;

	/** Input action for camera look (2D axis: yaw/pitch). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Look;

	/** Input action for jumping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Jump;

	/** Input action for sprinting (hold). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Sprint;

	// ---------------------------------------------------------------
	//  Movement Settings
	// ---------------------------------------------------------------

	/** Walking speed in cm/s. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Movement", meta = (ClampMin = "0.0"))
	float WalkSpeed;

	/** Sprinting speed in cm/s. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Movement", meta = (ClampMin = "0.0"))
	float SprintSpeed;

private:
	// ---------------------------------------------------------------
	//  Input Handlers
	// ---------------------------------------------------------------

	/** Handles movement input (WASD / left stick). */
	void HandleMove(const FInputActionValue& Value);

	/** Handles look input (mouse / right stick). */
	void HandleLook(const FInputActionValue& Value);

	/** Handles jump start. */
	void HandleJumpStart();

	/** Handles jump stop. */
	void HandleJumpStop();

	/** Begins sprinting. */
	void HandleSprintStart();

	/** Stops sprinting. */
	void HandleSprintStop();

	/** Updates the character movement component's max walk speed based on sprint state. */
	void UpdateMovementSpeed();

	/** Whether the character is currently sprinting. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zoo|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;
};
