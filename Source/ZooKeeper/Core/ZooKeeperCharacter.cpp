#include "ZooKeeperCharacter.h"

#include "ZooKeeper.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Interaction/InteractionComponent.h"
#include "Player/ToolComponent.h"

AZooKeeperCharacter::AZooKeeperCharacter()
	: WalkSpeed(400.0f)
	, SprintSpeed(800.0f)
	, bIsSprinting(false)
{
	// ---------------------------------------------------------------
	//  Capsule Configuration
	// ---------------------------------------------------------------
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	// ---------------------------------------------------------------
	//  First-Person Camera
	// ---------------------------------------------------------------
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // Eye height
	FirstPersonCamera->bUsePawnControlRotation = true;

	// ---------------------------------------------------------------
	//  Interaction Component
	// ---------------------------------------------------------------
	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComp"));

	// ---------------------------------------------------------------
	//  Tool Component
	// ---------------------------------------------------------------
	ToolComp = CreateDefaultSubobject<UToolComponent>(TEXT("ToolComp"));

	// ---------------------------------------------------------------
	//  Movement Defaults
	// ---------------------------------------------------------------
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
		MoveComp->BrakingDecelerationWalking = 2000.0f;
		MoveComp->AirControl = 0.35f;
		MoveComp->JumpZVelocity = 420.0f;
		MoveComp->GravityScale = 1.0f;
	}

	// Don't rotate the character with the camera; only the camera rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}

void AZooKeeperCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogZooKeeper, Log, TEXT("ZooKeeperCharacter::BeginPlay - Character initialized. WalkSpeed=%.0f SprintSpeed=%.0f"), WalkSpeed, SprintSpeed);
}

void AZooKeeperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ---------------------------------------------------------------------------
//  Input Binding
// ---------------------------------------------------------------------------

void AZooKeeperCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooKeeperCharacter::SetupPlayerInputComponent - InputComponent is not UEnhancedInputComponent."));
		return;
	}

	// Movement (2D axis, triggered every frame while held)
	if (IA_Move)
	{
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZooKeeperCharacter::HandleMove);
	}

	// Look (2D axis, triggered every frame while active)
	if (IA_Look)
	{
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AZooKeeperCharacter::HandleLook);
	}

	// Jump (press/release)
	if (IA_Jump)
	{
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleJumpStart);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AZooKeeperCharacter::HandleJumpStop);
	}

	// Sprint (hold)
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleSprintStart);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AZooKeeperCharacter::HandleSprintStop);
	}

	// Tool scroll (mouse wheel)
	if (IA_ToolScroll)
	{
		EnhancedInput->BindAction(IA_ToolScroll, ETriggerEvent::Triggered, this, &AZooKeeperCharacter::HandleToolScroll);
	}

	// Tool selection by number key
	if (IA_Tool1)
	{
		EnhancedInput->BindAction(IA_Tool1, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleSelectTool1);
	}
	if (IA_Tool2)
	{
		EnhancedInput->BindAction(IA_Tool2, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleSelectTool2);
	}
	if (IA_Tool3)
	{
		EnhancedInput->BindAction(IA_Tool3, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleSelectTool3);
	}
	if (IA_Tool4)
	{
		EnhancedInput->BindAction(IA_Tool4, ETriggerEvent::Started, this, &AZooKeeperCharacter::HandleSelectTool4);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooKeeperCharacter::SetupPlayerInputComponent - Enhanced Input actions bound to character."));
}

// ---------------------------------------------------------------------------
//  Movement Input Handlers
// ---------------------------------------------------------------------------

void AZooKeeperCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (Controller)
	{
		// Get the controller's forward and right directions projected onto the ground plane
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// MoveInput.Y = Forward/Backward, MoveInput.X = Right/Left
		AddMovementInput(ForwardDir, MoveInput.Y);
		AddMovementInput(RightDir, MoveInput.X);
	}
}

void AZooKeeperCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void AZooKeeperCharacter::HandleJumpStart()
{
	Jump();
}

void AZooKeeperCharacter::HandleJumpStop()
{
	StopJumping();
}

void AZooKeeperCharacter::HandleSprintStart()
{
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void AZooKeeperCharacter::HandleSprintStop()
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void AZooKeeperCharacter::UpdateMovementSpeed()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	}
}

// ---------------------------------------------------------------------------
//  Tool Input Handlers
// ---------------------------------------------------------------------------

void AZooKeeperCharacter::HandleToolScroll(const FInputActionValue& Value)
{
	if (!ToolComp)
	{
		return;
	}

	const float ScrollValue = Value.Get<float>();
	if (ScrollValue > 0.0f)
	{
		ToolComp->CycleToolForward();
	}
	else if (ScrollValue < 0.0f)
	{
		ToolComp->CycleToolBackward();
	}
}

void AZooKeeperCharacter::HandleSelectTool1()
{
	if (ToolComp) { ToolComp->SetToolByIndex(0); }
}

void AZooKeeperCharacter::HandleSelectTool2()
{
	if (ToolComp) { ToolComp->SetToolByIndex(1); }
}

void AZooKeeperCharacter::HandleSelectTool3()
{
	if (ToolComp) { ToolComp->SetToolByIndex(2); }
}

void AZooKeeperCharacter::HandleSelectTool4()
{
	if (ToolComp) { ToolComp->SetToolByIndex(3); }
}
