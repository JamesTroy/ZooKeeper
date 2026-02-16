#include "ZooEnvironmentSetup.h"

#include "ZooKeeper.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AZooEnvironmentSetup::AZooEnvironmentSetup() {
  PrimaryActorTick.bCanEverTick = false;

  // ---------------------------------------------------------------
  //  Root
  // ---------------------------------------------------------------
  USceneComponent *Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  // ---------------------------------------------------------------
  //  Directional Light (Sun)
  // ---------------------------------------------------------------
  SunLight =
      CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
  SunLight->SetupAttachment(Root);

  SunIntensity = 10.0f; // lux – good outdoor default
  SunColor = FLinearColor(1.0f, 0.95f, 0.85f, 1.0f); // warm sunlight
  SunRotation = FRotator(-45.0f, -30.0f, 0.0f); // 45° elevation, slight offset

  SunLight->Intensity = SunIntensity;
  SunLight->SetLightColor(SunColor);
  SunLight->SetWorldRotation(SunRotation);
  SunLight->SetAtmosphereSunLight(true);
  SunLight->bUsedAsAtmosphereSunLight = true;
  SunLight->SetCastShadows(true);

  // ---------------------------------------------------------------
  //  Sky Atmosphere
  // ---------------------------------------------------------------
  SkyAtmosphere =
      CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
  SkyAtmosphere->SetupAttachment(Root);

  // ---------------------------------------------------------------
  //  Sky Light
  // ---------------------------------------------------------------
  SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
  SkyLight->SetupAttachment(Root);

  SkyLightIntensity = 1.0f;
  SkyLight->Intensity = SkyLightIntensity;
  SkyLight->bRealTimeCapture = true; // picks up sky atmosphere automatically
  SkyLight->bLowerHemisphereIsBlack = false;
  SkyLight->SourceType = ESkyLightSourceType::SLS_CapturedScene;

  // ---------------------------------------------------------------
  //  Exponential Height Fog
  // ---------------------------------------------------------------
  HeightFog =
      CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
  HeightFog->SetupAttachment(Root);

  FogDensity = 0.005f;
  FogColor = FLinearColor(0.7f, 0.8f, 0.9f, 1.0f); // soft blue haze

  HeightFog->SetFogDensity(FogDensity);
  HeightFog->SetFogInscatteringColor(FogColor);
  HeightFog->SetVolumetricFog(true);

  // ---------------------------------------------------------------
  //  Ground Plane
  // ---------------------------------------------------------------
  GroundPlane =
      CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundPlane"));
  GroundPlane->SetupAttachment(Root);

  GroundScale = 200.0f; // 200 × 100 cm × 2 sides = 200 m × 200 m
  GroundColor = FLinearColor(0.15f, 0.45f, 0.10f, 1.0f); // grass green

  // Load the built-in engine plane mesh
  static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
      TEXT("/Engine/BasicShapes/Plane"));
  if (PlaneMesh.Succeeded()) {
    GroundPlane->SetStaticMesh(PlaneMesh.Object);
  }

  GroundPlane->SetWorldScale3D(FVector(GroundScale, GroundScale, 1.0f));
  GroundPlane->SetWorldLocation(FVector(
      0.0f, 0.0f, -5.0f)); // slightly below origin so player stands on it
  GroundPlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  GroundPlane->SetCollisionResponseToAllChannels(ECR_Block);
  GroundPlane->SetMobility(EComponentMobility::Static);
}

void AZooEnvironmentSetup::BeginPlay() {
  Super::BeginPlay();

  // Apply tunable values that may have been overridden in a Blueprint subclass
  if (SunLight) {
    SunLight->SetIntensity(SunIntensity);
    SunLight->SetLightColor(SunColor);
    SunLight->SetWorldRotation(SunRotation);
  }

  if (SkyLight) {
    SkyLight->SetIntensity(SkyLightIntensity);
    SkyLight->RecaptureSky();
  }

  if (HeightFog) {
    HeightFog->SetFogDensity(FogDensity);
    HeightFog->SetFogInscatteringColor(FogColor);
  }

  ApplyGroundMaterial();

  UE_LOG(LogZooKeeper, Log,
         TEXT("ZooEnvironmentSetup - Environment actors initialized."));
}

void AZooEnvironmentSetup::ApplyGroundMaterial() {
  if (!GroundPlane || !GroundPlane->GetStaticMesh()) {
    return;
  }

  // Create a simple dynamic material and tint it with GroundColor
  UMaterialInterface *BaseMaterial = GroundPlane->GetMaterial(0);
  if (BaseMaterial) {
    UMaterialInstanceDynamic *DynMat =
        UMaterialInstanceDynamic::Create(BaseMaterial, this);
    if (DynMat) {
      DynMat->SetVectorParameterValue(TEXT("BaseColor"), GroundColor);
      // Also try the common "Color" parameter name as a fallback
      DynMat->SetVectorParameterValue(TEXT("Color"), GroundColor);
      GroundPlane->SetMaterial(0, DynMat);
    }
  }
}
