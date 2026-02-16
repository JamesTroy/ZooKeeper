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

  // Root
  USceneComponent *Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  // ---- Directional Light (Sun) ----
  SunLight =
      CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
  SunLight->SetupAttachment(Root);
  SunLight->Intensity = 10.0f;
  SunLight->LightColor = FColor(255, 242, 217); // warm sunlight
  SunLight->SetRelativeRotation(FRotator(-45.0f, -30.0f, 0.0f));
  SunLight->bAtmosphereSunLight = true;
  SunLight->CastShadows = true;
  SunLight->CastDynamicShadows = true;

  // ---- Sky Atmosphere ----
  SkyAtmosphere =
      CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
  SkyAtmosphere->SetupAttachment(Root);

  // ---- Sky Light ----
  SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
  SkyLight->SetupAttachment(Root);
  SkyLight->Intensity = 1.0f;
  SkyLight->bLowerHemisphereIsBlack = false;

  // ---- Exponential Height Fog ----
  HeightFog =
      CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
  HeightFog->SetupAttachment(Root);
  HeightFog->FogDensity = 0.003f;
  HeightFog->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 0.9f, 1.0f));
  HeightFog->FogHeightFalloff = 0.2f;

  // ---- Ground Plane ----
  GroundPlane =
      CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundPlane"));
  GroundPlane->SetupAttachment(Root);

  static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
      TEXT("/Engine/BasicShapes/Plane"));
  if (PlaneMesh.Succeeded()) {
    GroundPlane->SetStaticMesh(PlaneMesh.Object);
  }

  GroundPlane->SetRelativeScale3D(FVector(300.0f, 300.0f, 1.0f)); // 300m x 300m
  GroundPlane->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
  GroundPlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  GroundPlane->SetCollisionResponseToAllChannels(ECR_Block);
  GroundPlane->SetMobility(EComponentMobility::Static);
}

void AZooEnvironmentSetup::BeginPlay() {
  Super::BeginPlay();

  // Recapture sky after everything is initialized
  if (SkyLight) {
    SkyLight->RecaptureSky();
  }

  ApplyGroundMaterial();

  UE_LOG(LogZooKeeper, Log,
         TEXT("ZooEnvironmentSetup - Environment initialized."));
}

void AZooEnvironmentSetup::ApplyGroundMaterial() {
  if (!GroundPlane || !GroundPlane->GetStaticMesh()) {
    return;
  }

  UMaterialInterface *BaseMaterial = GroundPlane->GetMaterial(0);
  if (BaseMaterial) {
    UMaterialInstanceDynamic *DynMat =
        UMaterialInstanceDynamic::Create(BaseMaterial, this);
    if (DynMat) {
      // Grass green color
      FLinearColor GrassColor(0.08f, 0.35f, 0.05f, 1.0f);
      DynMat->SetVectorParameterValue(TEXT("BaseColor"), GrassColor);
      DynMat->SetVectorParameterValue(TEXT("Color"), GrassColor);
      GroundPlane->SetMaterial(0, DynMat);
    }
  }
}
