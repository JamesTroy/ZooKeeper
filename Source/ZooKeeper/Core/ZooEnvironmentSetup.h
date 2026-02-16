#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZooEnvironmentSetup.generated.h"

class UDirectionalLightComponent;
class USkyAtmosphereComponent;
class USkyLightComponent;
class UExponentialHeightFogComponent;
class UStaticMeshComponent;

/**
 * AZooEnvironmentSetup
 *
 * Spawned by the game mode to provide essential lighting, sky, fog,
 * and a ground plane so that any empty map is immediately playable.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Environment Setup"))
class ZOOKEEPER_API AZooEnvironmentSetup : public AActor {
  GENERATED_BODY()

public:
  AZooEnvironmentSetup();

protected:
  virtual void BeginPlay() override;

  // --- Lighting ---
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
  TObjectPtr<UDirectionalLightComponent> SunLight;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
  TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
  TObjectPtr<USkyLightComponent> SkyLight;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
  TObjectPtr<UExponentialHeightFogComponent> HeightFog;

  // --- Ground ---
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
  TObjectPtr<UStaticMeshComponent> GroundPlane;

private:
  void ApplyGroundMaterial();
};
