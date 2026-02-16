#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZooPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/** Broadcast when build mode is toggled on or off. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildModeChanged, bool,
                                            bBuildModeActive);

/**
 * AZooPlayerController
 *
 * Player controller for the Zoo Keeper game. Manages Enhanced Input setup,
 * build-mode toggling, and input mapping context registration.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Player Controller"))
class ZOOKEEPER_API AZooPlayerController : public APlayerController {
  GENERATED_BODY()

public:
  AZooPlayerController();

  //~ Begin APlayerController Interface
  virtual void BeginPlay() override;
  virtual void SetupInputComponent() override;
  //~ End APlayerController Interface

  /** Returns whether the player is currently in build mode. */
  UFUNCTION(BlueprintPure, Category = "Zoo|BuildMode")
  bool IsInBuildMode() const { return bIsInBuildMode; }

  // ---------------------------------------------------------------
  //  Delegates
  // ---------------------------------------------------------------

  /** Fired when build mode is toggled. */
  UPROPERTY(BlueprintAssignable, Category = "Zoo|BuildMode")
  FOnBuildModeChanged OnBuildModeChanged;

protected:
  // ---------------------------------------------------------------
  //  Enhanced Input Assets (assigned in Blueprint or via defaults)
  // ---------------------------------------------------------------

  /** The default input mapping context applied at BeginPlay. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputMappingContext> DefaultMappingContext;

  /** Input mapping context active during build mode (Place, Cancel, Rotate,
   * Demolish). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputMappingContext> BuildModeMappingContext;

  /** Input action for movement (2D axis). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Move;

  /** Input action for camera look (2D axis). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Look;

  /** Input action for jumping. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Jump;

  /** Input action for interacting with objects. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Interact;

  /** Input action for sprinting (hold). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Sprint;

  /** Input action for toggling build mode. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_ToggleBuildMode;

  /** Input action for pausing the game (Escape). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Pause;

  /** Input action for opening the management screen (Tab). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input")
  TObjectPtr<UInputAction> IA_Management;

  /** Priority for the default mapping context. Higher values take precedence.
   */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input",
            meta = (ClampMin = "0"))
  int32 DefaultMappingPriority;

  /** Priority for the build mode mapping context. Should be higher than
   * default. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Input",
            meta = (ClampMin = "0"))
  int32 BuildModeMappingPriority;

  /** Quick save (F5). */
  UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
  void QuickSave();

  /** Quick load (F9). */
  UFUNCTION(BlueprintCallable, Category = "Zoo|SaveLoad")
  void QuickLoad();

private:
  /** Handles the ToggleBuildMode input action. */
  void HandleToggleBuildMode();

  /** Handles the Pause input action. */
  void HandlePause();

  /** Handles the Management screen input action. */
  void HandleManagement();

  /**
   * Creates UInputAction and UInputMappingContext objects at runtime
   * so the character has working input without any Blueprint/uasset files.
   * Only called when DefaultMappingContext is null (no Blueprint override).
   */
  void CreateDefaultInputAssets();

  /** Whether the player is currently in build mode. */
  UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zoo|BuildMode",
            meta = (AllowPrivateAccess = "true"))
  bool bIsInBuildMode;

  /** Whether the pause menu is currently shown. */
  bool bIsPauseMenuOpen;
};
