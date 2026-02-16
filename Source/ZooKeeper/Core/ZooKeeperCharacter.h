#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZooKeeperCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInteractionComponent;
class UToolComponent;
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

	/** Returns the tool component. */
	UFUNCTION(BlueprintPure, Category = "Zoo|Tools")
	UToolComponent* GetToolComponent() const { return ToolComp; }

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

	/** Manages the currently equipped tool (Hand, FoodBucket, BuildTool, etc.). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Tools", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UToolComponent> ToolComp;

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

	/** Input action for scrolling through tools (mouse wheel). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_ToolScroll;

	/** Input action for selecting tool 1 (Hand). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Tool1;

	/** Input action for selecting tool 2 (Food Bucket). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Tool2;

	/** Input action for selecting tool 3 (Build Tool). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Tool3;

	/** Input action for selecting tool 4 (Binoculars). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
	TObjectPtr<UInputAction> IA_Tool4;

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

	/** Handles tool scroll wheel input. */
	void HandleToolScroll(const FInputActionValue& Value);

	/** Select tool by number key. */
	void HandleSelectTool1();
	void HandleSelectTool2();
	void HandleSelectTool3();
	void HandleSelectTool4();

	/** Whether the character is currently sprinting. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zoo|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;
};
