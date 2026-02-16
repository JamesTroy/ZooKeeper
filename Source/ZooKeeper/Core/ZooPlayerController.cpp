#include "ZooPlayerController.h"

#include "ZooKeeper.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "SaveLoad/ZooSaveSubsystem.h"
#include "Subsystems/TimeSubsystem.h"

AZooPlayerController::AZooPlayerController()
	: DefaultMappingPriority(0)
	, BuildModeMappingPriority(1)
	, bIsInBuildMode(false)
	, bIsPauseMenuOpen(false)
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

	// Pause (Escape)
	if (IA_Pause)
	{
		EnhancedInput->BindAction(IA_Pause, ETriggerEvent::Started, this, &AZooPlayerController::HandlePause);
	}

	// Management screen (Tab)
	if (IA_Management)
	{
		EnhancedInput->BindAction(IA_Management, ETriggerEvent::Started, this, &AZooPlayerController::HandleManagement);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController::SetupInputComponent - Enhanced Input actions bound."));
}

void AZooPlayerController::HandleToggleBuildMode()
{
	bIsInBuildMode = !bIsInBuildMode;

	// Swap input mapping context for build mode
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (BuildModeMappingContext)
		{
			if (bIsInBuildMode)
			{
				Subsystem->AddMappingContext(BuildModeMappingContext, BuildModeMappingPriority);
			}
			else
			{
				Subsystem->RemoveMappingContext(BuildModeMappingContext);
			}
		}
	}

	OnBuildModeChanged.Broadcast(bIsInBuildMode);

	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Build Mode %s"), bIsInBuildMode ? TEXT("ENABLED") : TEXT("DISABLED"));
}

void AZooPlayerController::HandlePause()
{
	bIsPauseMenuOpen = !bIsPauseMenuOpen;

	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			if (bIsPauseMenuOpen)
			{
				TimeSys->PauseTime();
			}
			else
			{
				TimeSys->ResumeTime();
			}
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Pause %s."),
		bIsPauseMenuOpen ? TEXT("OPENED") : TEXT("CLOSED"));
}

void AZooPlayerController::HandleManagement()
{
	UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Management screen toggled."));
}

void AZooPlayerController::QuickSave()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			SaveSub->SaveGame(TEXT("Quicksave"));
			UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Quick saved."));
		}
	}
}

void AZooPlayerController::QuickLoad()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UZooSaveSubsystem* SaveSub = GI->GetSubsystem<UZooSaveSubsystem>())
		{
			if (SaveSub->LoadGame(TEXT("Quicksave")))
			{
				UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Quick loaded."));
			}
		}
	}
}
