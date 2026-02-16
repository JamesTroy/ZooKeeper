#include "ZooPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "SaveLoad/ZooSaveSubsystem.h"
#include "Subsystems/TimeSubsystem.h"
#include "ZooKeeper.h"
#include "ZooKeeperCharacter.h"

AZooPlayerController::AZooPlayerController()
    : DefaultMappingPriority(0), BuildModeMappingPriority(1),
      bIsInBuildMode(false), bIsPauseMenuOpen(false) {}

void AZooPlayerController::BeginPlay() {
  Super::BeginPlay();

  // If no Blueprint-assigned mapping context, create input assets
  // programmatically.
  if (!DefaultMappingContext) {
    UE_LOG(LogZooKeeper, Log,
           TEXT("ZooPlayerController::BeginPlay - No DefaultMappingContext "
                "set. Creating input assets programmatically."));
    CreateDefaultInputAssets();
  }

  // Add the default mapping context to the local player's Enhanced Input
  // subsystem.
  if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
          ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
              GetLocalPlayer())) {
    if (DefaultMappingContext) {
      Subsystem->AddMappingContext(DefaultMappingContext,
                                   DefaultMappingPriority);
      UE_LOG(LogZooKeeper, Log,
             TEXT("ZooPlayerController::BeginPlay - Added default mapping "
                  "context with priority %d."),
             DefaultMappingPriority);
    } else {
      UE_LOG(LogZooKeeper, Warning,
             TEXT("ZooPlayerController::BeginPlay - DefaultMappingContext is "
                  "still null after creation attempt."));
    }
  } else {
    UE_LOG(LogZooKeeper, Warning,
           TEXT("ZooPlayerController::BeginPlay - Could not find "
                "EnhancedInputLocalPlayerSubsystem."));
  }
}

void AZooPlayerController::SetupInputComponent() {
  Super::SetupInputComponent();

  UEnhancedInputComponent *EnhancedInput =
      Cast<UEnhancedInputComponent>(InputComponent);
  if (!EnhancedInput) {
    UE_LOG(LogZooKeeper, Error,
           TEXT("ZooPlayerController::SetupInputComponent - InputComponent is "
                "not UEnhancedInputComponent. "
                "Ensure the project is configured for Enhanced Input."));
    return;
  }

  // Build mode toggle (controller-level, so it works regardless of possessed
  // pawn)
  if (IA_ToggleBuildMode) {
    EnhancedInput->BindAction(IA_ToggleBuildMode, ETriggerEvent::Started, this,
                              &AZooPlayerController::HandleToggleBuildMode);
  }

  // Pause (Escape)
  if (IA_Pause) {
    EnhancedInput->BindAction(IA_Pause, ETriggerEvent::Started, this,
                              &AZooPlayerController::HandlePause);
  }

  // Management screen (Tab)
  if (IA_Management) {
    EnhancedInput->BindAction(IA_Management, ETriggerEvent::Started, this,
                              &AZooPlayerController::HandleManagement);
  }

  UE_LOG(LogZooKeeper, Log,
         TEXT("ZooPlayerController::SetupInputComponent - Enhanced Input "
              "actions bound."));
}

void AZooPlayerController::HandleToggleBuildMode() {
  bIsInBuildMode = !bIsInBuildMode;

  // Swap input mapping context for build mode
  if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
          ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
              GetLocalPlayer())) {
    if (BuildModeMappingContext) {
      if (bIsInBuildMode) {
        Subsystem->AddMappingContext(BuildModeMappingContext,
                                     BuildModeMappingPriority);
      } else {
        Subsystem->RemoveMappingContext(BuildModeMappingContext);
      }
    }
  }

  OnBuildModeChanged.Broadcast(bIsInBuildMode);

  UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Build Mode %s"),
         bIsInBuildMode ? TEXT("ENABLED") : TEXT("DISABLED"));
}

void AZooPlayerController::HandlePause() {
  bIsPauseMenuOpen = !bIsPauseMenuOpen;

  if (UWorld *World = GetWorld()) {
    if (UTimeSubsystem *TimeSys = World->GetSubsystem<UTimeSubsystem>()) {
      if (bIsPauseMenuOpen) {
        TimeSys->PauseTime();
      } else {
        TimeSys->ResumeTime();
      }
    }
  }

  UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Pause %s."),
         bIsPauseMenuOpen ? TEXT("OPENED") : TEXT("CLOSED"));
}

void AZooPlayerController::HandleManagement() {
  UE_LOG(LogZooKeeper, Log,
         TEXT("ZooPlayerController - Management screen toggled."));
}

void AZooPlayerController::QuickSave() {
  if (UGameInstance *GI = GetGameInstance()) {
    if (UZooSaveSubsystem *SaveSub = GI->GetSubsystem<UZooSaveSubsystem>()) {
      SaveSub->SaveGame(TEXT("Quicksave"));
      UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Quick saved."));
    }
  }
}

void AZooPlayerController::QuickLoad() {
  if (UGameInstance *GI = GetGameInstance()) {
    if (UZooSaveSubsystem *SaveSub = GI->GetSubsystem<UZooSaveSubsystem>()) {
      if (SaveSub->LoadGame(TEXT("Quicksave"))) {
        UE_LOG(LogZooKeeper, Log, TEXT("ZooPlayerController - Quick loaded."));
      }
    }
  }
}

void AZooPlayerController::CreateDefaultInputAssets() {
  // ---------------------------------------------------------------
  //  Create Input Actions
  // ---------------------------------------------------------------

  // Move (2D Axis: WASD)
  UInputAction *MoveAction =
      NewObject<UInputAction>(this, TEXT("IA_Move_Default"));
  MoveAction->ValueType = EInputActionValueType::Axis2D;

  // Look (2D Axis: Mouse)
  UInputAction *LookAction =
      NewObject<UInputAction>(this, TEXT("IA_Look_Default"));
  LookAction->ValueType = EInputActionValueType::Axis2D;

  // Jump (Digital/Bool: Space)
  UInputAction *JumpAction =
      NewObject<UInputAction>(this, TEXT("IA_Jump_Default"));
  JumpAction->ValueType = EInputActionValueType::Boolean;

  // Sprint (Digital/Bool: Shift)
  UInputAction *SprintAction =
      NewObject<UInputAction>(this, TEXT("IA_Sprint_Default"));
  SprintAction->ValueType = EInputActionValueType::Boolean;

  // ---------------------------------------------------------------
  //  Store references on controller properties
  // ---------------------------------------------------------------
  IA_Move = MoveAction;
  IA_Look = LookAction;
  IA_Jump = JumpAction;
  IA_Sprint = SprintAction;

  // ---------------------------------------------------------------
  //  Create Input Mapping Context
  // ---------------------------------------------------------------
  DefaultMappingContext =
      NewObject<UInputMappingContext>(this, TEXT("IMC_Default"));

  // --- WASD Movement ---
  // W = Forward (+Y)
  {
    FEnhancedActionKeyMapping &Mapping =
        DefaultMappingContext->MapKey(MoveAction, EKeys::W);
    UInputModifierSwizzleAxis *Swizzle =
        NewObject<UInputModifierSwizzleAxis>(this);
    Swizzle->Order = EInputAxisSwizzle::YXZ;
    Mapping.Modifiers.Add(Swizzle);
  }
  // S = Backward (-Y)
  {
    FEnhancedActionKeyMapping &Mapping =
        DefaultMappingContext->MapKey(MoveAction, EKeys::S);
    UInputModifierSwizzleAxis *Swizzle =
        NewObject<UInputModifierSwizzleAxis>(this);
    Swizzle->Order = EInputAxisSwizzle::YXZ;
    Mapping.Modifiers.Add(Swizzle);
    UInputModifierNegate *Negate = NewObject<UInputModifierNegate>(this);
    Mapping.Modifiers.Add(Negate);
  }
  // D = Right (+X)
  {
    DefaultMappingContext->MapKey(MoveAction, EKeys::D);
  }
  // A = Left (-X)
  {
    FEnhancedActionKeyMapping &Mapping =
        DefaultMappingContext->MapKey(MoveAction, EKeys::A);
    UInputModifierNegate *Negate = NewObject<UInputModifierNegate>(this);
    Mapping.Modifiers.Add(Negate);
  }

  // --- Mouse Look ---
  // Mouse X = Yaw
  {
    DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);
  }

  // --- Jump ---
  DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);

  // --- Sprint ---
  DefaultMappingContext->MapKey(SprintAction, EKeys::LeftShift);

  // ---------------------------------------------------------------
  //  Assign input actions to the possessed character and re-bind
  // ---------------------------------------------------------------
  AZooKeeperCharacter *ZooChar = Cast<AZooKeeperCharacter>(GetPawn());
  if (ZooChar) {
    ZooChar->IA_Move = MoveAction;
    ZooChar->IA_Look = LookAction;
    ZooChar->IA_Jump = JumpAction;
    ZooChar->IA_Sprint = SprintAction;

    // The character's SetupPlayerInputComponent() already ran during
    // possession, but at that point all IA_ pointers were null.
    // Clear the existing bindings and re-run setup with the new actions.
    if (ZooChar->InputComponent) {
      UEnhancedInputComponent *EIC =
          Cast<UEnhancedInputComponent>(ZooChar->InputComponent);
      if (EIC) {
        EIC->ClearActionBindings();
      }
      ZooChar->SetupPlayerInputComponent(ZooChar->InputComponent);
      UE_LOG(
          LogZooKeeper, Log,
          TEXT("ZooPlayerController::CreateDefaultInputAssets - Input actions "
               "assigned and re-bound to character."));
    } else {
      UE_LOG(
          LogZooKeeper, Warning,
          TEXT("ZooPlayerController::CreateDefaultInputAssets - Character has "
               "no InputComponent."));
    }
  } else {
    UE_LOG(LogZooKeeper, Warning,
           TEXT("ZooPlayerController::CreateDefaultInputAssets - No "
                "ZooKeeperCharacter possessed yet."));
  }
}
