#include "ZooPlayerController.h"

#include "ZooKeeper.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AZooPlayerController::AZooPlayerController()
	: DefaultMappingPriority(0)
	, bIsInBuildMode(false)
{
}

void AZooPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add the default mapping context to the local player's Enhanced Input subsystem.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, DefaultMappingPriority);
			UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController::BeginPlay - Added default mapping context with priority %d."), DefaultMappingPriority);
		}
		else
		{
			UE_LOG(LogZooKeeper, Warning, TEXT("ZooPlayerController::BeginPlay - DefaultMappingContext is not set. Assign it in the Blueprint."));
		}
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooPlayerController::BeginPlay - Could not find EnhancedInputLocalPlayerSubsystem."));
	}
}

void AZooPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooPlayerController::SetupInputComponent - InputComponent is not UEnhancedInputComponent. "
			"Ensure the project is configured for Enhanced Input."));
		return;
	}

	// Build mode toggle (controller-level, so it works regardless of possessed pawn)
	if (IA_ToggleBuildMode)
	{
		EnhancedInput->BindAction(IA_ToggleBuildMode, ETriggerEvent::Started, this, &AZooPlayerController::HandleToggleBuildMode);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController::SetupInputComponent - Enhanced Input actions bound."));
}

void AZooPlayerController::HandleToggleBuildMode()
{
	bIsInBuildMode = !bIsInBuildMode;
	OnBuildModeChanged.Broadcast(bIsInBuildMode);

	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Build Mode %s"), bIsInBuildMode ? TEXT("ENABLED") : TEXT("DISABLED"));
}
