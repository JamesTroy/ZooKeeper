#include "ZooLevelBuilder.h"

#include "ZooKeeper.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AZooLevelBuilder::AZooLevelBuilder() : BlockCounter(0) {
  PrimaryActorTick.bCanEverTick = false;

  USceneComponent *Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  // Cache basic shape meshes
  static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
      TEXT("/Engine/BasicShapes/Cube"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
      TEXT("/Engine/BasicShapes/Cylinder"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(
      TEXT("/Engine/BasicShapes/Sphere"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(
      TEXT("/Engine/BasicShapes/Plane"));

  if (CubeFinder.Succeeded())
    CubeMesh = CubeFinder.Object;
  if (CylinderFinder.Succeeded())
    CylinderMesh = CylinderFinder.Object;
  if (SphereFinder.Succeeded())
    SphereMesh = SphereFinder.Object;
  if (PlaneFinder.Succeeded())
    PlaneMesh = PlaneFinder.Object;
}

void AZooLevelBuilder::BeginPlay() {
  Super::BeginPlay();

  BuildEntrance();
  BuildPaths();
  BuildEnclosures();
  BuildTrees();
  BuildBenches();
  BuildPond();
  BuildInfoSigns();
  BuildDecorations();

  UE_LOG(LogZooKeeper, Log,
         TEXT("ZooLevelBuilder - Zoo layout built with %d blocks."),
         BlockCounter);
}

// ---------------------------------------------------------------------------
//  Helper: Create a colored mesh block
// ---------------------------------------------------------------------------
UStaticMeshComponent *AZooLevelBuilder::CreateBlock(
    const FString &Name, UStaticMesh *Mesh, const FVector &Location,
    const FVector &Scale, const FLinearColor &Color, const FRotator &Rotation) {
  if (!Mesh)
    return nullptr;

  FName CompName(*FString::Printf(TEXT("Block_%d_%s"), BlockCounter++, *Name));

  UStaticMeshComponent *Comp = NewObject<UStaticMeshComponent>(this, CompName);
  if (!Comp)
    return nullptr;

  Comp->SetStaticMesh(Mesh);
  Comp->SetWorldLocation(Location);
  Comp->SetWorldScale3D(Scale);
  Comp->SetWorldRotation(Rotation);
  Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  Comp->SetCollisionResponseToAllChannels(ECR_Block);
  Comp->SetMobility(EComponentMobility::Movable);
  Comp->RegisterComponent();
  Comp->AttachToComponent(GetRootComponent(),
                          FAttachmentTransformRules::KeepWorldTransform);

  // Apply color via dynamic material
  UMaterialInterface *BaseMat = Comp->GetMaterial(0);
  if (BaseMat) {
    UMaterialInstanceDynamic *DynMat =
        UMaterialInstanceDynamic::Create(BaseMat, this);
    if (DynMat) {
      DynMat->SetVectorParameterValue(TEXT("BaseColor"), Color);
      DynMat->SetVectorParameterValue(TEXT("Color"), Color);
      Comp->SetMaterial(0, DynMat);
    }
  }

  return Comp;
}

// ---------------------------------------------------------------------------
//  Zoo Entrance — Two pillars + crossbeam + sign
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildEntrance() {
  const FLinearColor PillarColor(0.35f, 0.25f, 0.15f, 1.0f); // dark wood brown
  const FLinearColor BeamColor(0.45f, 0.30f, 0.15f, 1.0f);   // lighter wood
  const FLinearColor SignColor(0.15f, 0.55f, 0.15f, 1.0f);   // zoo green

  // Left pillar
  CreateBlock(TEXT("Entrance_PillarL"), CubeMesh,
              FVector(800.0f, -250.0f, 200.0f), FVector(0.8f, 0.8f, 4.0f),
              PillarColor);

  // Right pillar
  CreateBlock(TEXT("Entrance_PillarR"), CubeMesh,
              FVector(800.0f, 250.0f, 200.0f), FVector(0.8f, 0.8f, 4.0f),
              PillarColor);

  // Top crossbeam
  CreateBlock(TEXT("Entrance_Beam"), CubeMesh, FVector(800.0f, 0.0f, 430.0f),
              FVector(0.6f, 6.0f, 0.5f), BeamColor);

  // Sign on top
  CreateBlock(TEXT("Entrance_Sign"), CubeMesh, FVector(800.0f, 0.0f, 490.0f),
              FVector(0.3f, 4.0f, 0.8f), SignColor);

  // Decorative spheres on top of pillars
  const FLinearColor GoldColor(0.85f, 0.65f, 0.13f, 1.0f);
  CreateBlock(TEXT("Entrance_OrbL"), SphereMesh,
              FVector(800.0f, -250.0f, 430.0f), FVector(0.5f, 0.5f, 0.5f),
              GoldColor);
  CreateBlock(TEXT("Entrance_OrbR"), SphereMesh,
              FVector(800.0f, 250.0f, 430.0f), FVector(0.5f, 0.5f, 0.5f),
              GoldColor);
}

// ---------------------------------------------------------------------------
//  Paths — Main walkway + side paths
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildPaths() {
  const FLinearColor PathColor(0.6f, 0.55f, 0.45f, 1.0f); // sandy stone

  // Main path from entrance going forward (north)
  for (int32 i = 0; i < 20; ++i) {
    float X = 800.0f + (i * 300.0f);
    CreateBlock(*FString::Printf(TEXT("MainPath_%d"), i), CubeMesh,
                FVector(X, 0.0f, 2.0f), FVector(3.0f, 2.5f, 0.04f), PathColor);
  }

  // Left branch path
  for (int32 i = 0; i < 8; ++i) {
    float Y = -300.0f - (i * 300.0f);
    CreateBlock(*FString::Printf(TEXT("LeftPath_%d"), i), CubeMesh,
                FVector(2600.0f, Y, 2.0f), FVector(2.5f, 3.0f, 0.04f),
                PathColor);
  }

  // Right branch path
  for (int32 i = 0; i < 8; ++i) {
    float Y = 300.0f + (i * 300.0f);
    CreateBlock(*FString::Printf(TEXT("RightPath_%d"), i), CubeMesh,
                FVector(2600.0f, Y, 2.0f), FVector(2.5f, 3.0f, 0.04f),
                PathColor);
  }

  // Cross path connecting left and right branches
  for (int32 i = 0; i < 15; ++i) {
    float Y = -2100.0f + (i * 300.0f);
    CreateBlock(*FString::Printf(TEXT("CrossPath_%d"), i), CubeMesh,
                FVector(4400.0f, Y, 2.0f), FVector(2.5f, 3.0f, 0.04f),
                PathColor);
  }
}

// ---------------------------------------------------------------------------
//  Enclosures — Fenced rectangular areas
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildEnclosure(const FVector &Center,
                                      const FString &Label,
                                      const FLinearColor &FenceColor,
                                      float SizeX, float SizeY) {
  const float FenceHeight = 150.0f;
  const float FenceThickness = 0.15f;
  const float PostSize = 0.4f;

  // North wall
  CreateBlock(*FString::Printf(TEXT("%s_WallN"), *Label), CubeMesh,
              FVector(Center.X + SizeX / 2.0f, Center.Y, FenceHeight / 2.0f),
              FVector(FenceThickness, SizeY / 100.0f, FenceHeight / 100.0f),
              FenceColor);

  // South wall
  CreateBlock(*FString::Printf(TEXT("%s_WallS"), *Label), CubeMesh,
              FVector(Center.X - SizeX / 2.0f, Center.Y, FenceHeight / 2.0f),
              FVector(FenceThickness, SizeY / 100.0f, FenceHeight / 100.0f),
              FenceColor);

  // East wall
  CreateBlock(*FString::Printf(TEXT("%s_WallE"), *Label), CubeMesh,
              FVector(Center.X, Center.Y + SizeY / 2.0f, FenceHeight / 2.0f),
              FVector(SizeX / 100.0f, FenceThickness, FenceHeight / 100.0f),
              FenceColor);

  // West wall
  CreateBlock(*FString::Printf(TEXT("%s_WallW"), *Label), CubeMesh,
              FVector(Center.X, Center.Y - SizeY / 2.0f, FenceHeight / 2.0f),
              FVector(SizeX / 100.0f, FenceThickness, FenceHeight / 100.0f),
              FenceColor);

  // Corner posts
  const FLinearColor PostColor(0.3f, 0.3f, 0.3f, 1.0f); // dark gray
  FVector Corners[4] = {FVector(Center.X + SizeX / 2.0f,
                                Center.Y + SizeY / 2.0f, FenceHeight / 2.0f),
                        FVector(Center.X + SizeX / 2.0f,
                                Center.Y - SizeY / 2.0f, FenceHeight / 2.0f),
                        FVector(Center.X - SizeX / 2.0f,
                                Center.Y + SizeY / 2.0f, FenceHeight / 2.0f),
                        FVector(Center.X - SizeX / 2.0f,
                                Center.Y - SizeY / 2.0f, FenceHeight / 2.0f)};
  for (int32 i = 0; i < 4; ++i) {
    CreateBlock(*FString::Printf(TEXT("%s_Post%d"), *Label, i), CylinderMesh,
                Corners[i], FVector(PostSize, PostSize, FenceHeight / 100.0f),
                PostColor);
  }

  // Ground inside enclosure (dirt/sand color)
  const FLinearColor DirtColor(0.55f, 0.45f, 0.30f, 1.0f);
  CreateBlock(*FString::Printf(TEXT("%s_Ground"), *Label), PlaneMesh,
              FVector(Center.X, Center.Y, 3.0f),
              FVector(SizeX / 100.0f, SizeY / 100.0f, 1.0f), DirtColor);
}

void AZooLevelBuilder::BuildEnclosures() {
  // Enclosure 1: Lions (front-left) — warm orange fences
  BuildEnclosure(FVector(3500.0f, -1500.0f, 0.0f), TEXT("Lions"),
                 FLinearColor(0.7f, 0.4f, 0.1f, 1.0f), 1200.0f, 1000.0f);

  // Enclosure 2: Elephants (front-right) — gray fences
  BuildEnclosure(FVector(3500.0f, 1500.0f, 0.0f), TEXT("Elephants"),
                 FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), 1400.0f, 1200.0f);

  // Enclosure 3: Penguins (back-left) — blue fences
  BuildEnclosure(FVector(5500.0f, -1500.0f, 0.0f), TEXT("Penguins"),
                 FLinearColor(0.2f, 0.4f, 0.7f, 1.0f), 1000.0f, 1000.0f);

  // Enclosure 4: Monkeys (back-right) — green fences
  BuildEnclosure(FVector(5500.0f, 1500.0f, 0.0f), TEXT("Monkeys"),
                 FLinearColor(0.2f, 0.6f, 0.2f, 1.0f), 1200.0f, 1000.0f);
}

// ---------------------------------------------------------------------------
//  Trees — Cylinder trunk + Sphere canopy
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildTrees() {
  const FLinearColor TrunkColor(0.4f, 0.25f, 0.1f, 1.0f); // brown
  const FLinearColor LeafColor(0.1f, 0.5f, 0.1f, 1.0f);   // green
  const FLinearColor DarkLeaf(0.05f, 0.4f, 0.08f, 1.0f);  // darker green

  struct FTreePos {
    FVector Pos;
    float TrunkH;
    float CanopyR;
    FLinearColor Leaf;
  };
  TArray<FTreePos> Trees = {
      // Along entrance path
      {FVector(600.0f, -400.0f, 0.0f), 250.0f, 200.0f, LeafColor},
      {FVector(600.0f, 400.0f, 0.0f), 280.0f, 220.0f, DarkLeaf},
      {FVector(1200.0f, -400.0f, 0.0f), 220.0f, 180.0f, LeafColor},
      {FVector(1200.0f, 400.0f, 0.0f), 260.0f, 200.0f, DarkLeaf},
      // Along main path
      {FVector(2000.0f, -400.0f, 0.0f), 300.0f, 250.0f, LeafColor},
      {FVector(2000.0f, 400.0f, 0.0f), 270.0f, 230.0f, DarkLeaf},
      // Between enclosures
      {FVector(4500.0f, 0.0f, 0.0f), 320.0f, 260.0f, LeafColor},
      {FVector(4500.0f, -300.0f, 0.0f), 290.0f, 240.0f, DarkLeaf},
      {FVector(4500.0f, 300.0f, 0.0f), 310.0f, 250.0f, LeafColor},
      // Back area
      {FVector(6500.0f, -500.0f, 0.0f), 280.0f, 220.0f, DarkLeaf},
      {FVector(6500.0f, 500.0f, 0.0f), 300.0f, 240.0f, LeafColor},
      {FVector(6500.0f, 0.0f, 0.0f), 250.0f, 200.0f, DarkLeaf},
  };

  for (int32 i = 0; i < Trees.Num(); ++i) {
    const FTreePos &T = Trees[i];
    // Trunk
    CreateBlock(*FString::Printf(TEXT("Tree%d_Trunk"), i), CylinderMesh,
                FVector(T.Pos.X, T.Pos.Y, T.TrunkH / 2.0f),
                FVector(0.4f, 0.4f, T.TrunkH / 100.0f), TrunkColor);

    // Canopy
    CreateBlock(
        *FString::Printf(TEXT("Tree%d_Canopy"), i), SphereMesh,
        FVector(T.Pos.X, T.Pos.Y, T.TrunkH + T.CanopyR * 0.5f),
        FVector(T.CanopyR / 50.0f, T.CanopyR / 50.0f, T.CanopyR / 60.0f),
        T.Leaf);
  }
}

// ---------------------------------------------------------------------------
//  Benches
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildBenches() {
  const FLinearColor BenchColor(0.5f, 0.35f, 0.2f, 1.0f); // wood brown
  const FLinearColor LegColor(0.3f, 0.3f, 0.3f, 1.0f);    // metal gray

  struct FBenchPos {
    FVector Pos;
    FRotator Rot;
  };
  TArray<FBenchPos> Benches = {
      {FVector(1500.0f, -350.0f, 0.0f), FRotator(0, 0, 0)},
      {FVector(1500.0f, 350.0f, 0.0f), FRotator(0, 0, 0)},
      {FVector(3000.0f, -350.0f, 0.0f), FRotator(0, 0, 0)},
      {FVector(3000.0f, 350.0f, 0.0f), FRotator(0, 0, 0)},
      {FVector(4400.0f, -350.0f, 0.0f), FRotator(0, 90, 0)},
      {FVector(4400.0f, 350.0f, 0.0f), FRotator(0, 90, 0)},
  };

  for (int32 i = 0; i < Benches.Num(); ++i) {
    const FBenchPos &B = Benches[i];
    // Seat
    CreateBlock(*FString::Printf(TEXT("Bench%d_Seat"), i), CubeMesh,
                B.Pos + FVector(0, 0, 45.0f), FVector(1.2f, 0.5f, 0.08f),
                BenchColor, B.Rot);
    // Back rest
    CreateBlock(*FString::Printf(TEXT("Bench%d_Back"), i), CubeMesh,
                B.Pos + FVector(-20, 0, 80.0f), FVector(0.08f, 0.5f, 0.6f),
                BenchColor, B.Rot);
    // Legs
    CreateBlock(*FString::Printf(TEXT("Bench%d_LegL"), i), CubeMesh,
                B.Pos + FVector(0, -20, 22.0f), FVector(0.08f, 0.08f, 0.42f),
                LegColor, B.Rot);
    CreateBlock(*FString::Printf(TEXT("Bench%d_LegR"), i), CubeMesh,
                B.Pos + FVector(0, 20, 22.0f), FVector(0.08f, 0.08f, 0.42f),
                LegColor, B.Rot);
  }
}

// ---------------------------------------------------------------------------
//  Pond — Flat blue circle
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildPond() {
  const FLinearColor WaterColor(0.1f, 0.3f, 0.6f, 1.0f);
  const FLinearColor RockColor(0.4f, 0.4f, 0.38f, 1.0f);

  // Water surface
  CreateBlock(TEXT("Pond_Water"), CylinderMesh, FVector(4400.0f, 0.0f, 1.0f),
              FVector(4.0f, 4.0f, 0.02f), WaterColor);

  // Rocks around the pond
  for (int32 i = 0; i < 8; ++i) {
    float Angle = i * (360.0f / 8.0f);
    float Rad = FMath::DegreesToRadians(Angle);
    float RockX = 4400.0f + FMath::Cos(Rad) * 420.0f;
    float RockY = FMath::Sin(Rad) * 420.0f;
    float RockScale = 0.3f + FMath::FRand() * 0.3f;

    CreateBlock(*FString::Printf(TEXT("PondRock_%d"), i), SphereMesh,
                FVector(RockX, RockY, 15.0f),
                FVector(RockScale, RockScale, RockScale * 0.5f), RockColor);
  }
}

// ---------------------------------------------------------------------------
//  Info Signs — Thin tall boards near enclosures
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildInfoSigns() {
  const FLinearColor SignColor(0.85f, 0.75f, 0.55f, 1.0f); // light wood
  const FLinearColor PostColor(0.35f, 0.25f, 0.15f, 1.0f); // dark wood

  struct FSignInfo {
    FVector Pos;
    FRotator Rot;
  };
  TArray<FSignInfo> Signs = {
      {FVector(2800.0f, -1000.0f, 0.0f), FRotator(0, 0, 0)}, // Lions
      {FVector(2800.0f, 1000.0f, 0.0f), FRotator(0, 0, 0)},  // Elephants
      {FVector(4800.0f, -1000.0f, 0.0f), FRotator(0, 0, 0)}, // Penguins
      {FVector(4800.0f, 1000.0f, 0.0f), FRotator(0, 0, 0)},  // Monkeys
  };

  for (int32 i = 0; i < Signs.Num(); ++i) {
    const FSignInfo &S = Signs[i];
    // Post
    CreateBlock(*FString::Printf(TEXT("Sign%d_Post"), i), CylinderMesh,
                S.Pos + FVector(0, 0, 60.0f), FVector(0.1f, 0.1f, 1.2f),
                PostColor, S.Rot);
    // Board
    CreateBlock(*FString::Printf(TEXT("Sign%d_Board"), i), CubeMesh,
                S.Pos + FVector(0, 0, 130.0f), FVector(0.05f, 0.8f, 0.5f),
                SignColor, S.Rot);
  }
}

// ---------------------------------------------------------------------------
//  Decorations — Trash cans, lamp posts, flower beds
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildDecorations() {
  const FLinearColor TrashColor(0.3f, 0.35f, 0.3f, 1.0f); // gray-green
  const FLinearColor LampColor(0.2f, 0.2f, 0.2f, 1.0f);   // dark metal
  const FLinearColor LampBulb(1.0f, 0.95f, 0.7f, 1.0f);   // warm yellow
  const FLinearColor FlowerRed(0.7f, 0.1f, 0.1f, 1.0f);
  const FLinearColor FlowerYellow(0.9f, 0.8f, 0.1f, 1.0f);
  const FLinearColor SoilColor(0.3f, 0.2f, 0.1f, 1.0f);

  // Trash cans along paths
  TArray<FVector> TrashPositions = {
      FVector(1800.0f, 300.0f, 0.0f),
      FVector(1800.0f, -300.0f, 0.0f),
      FVector(3500.0f, 300.0f, 0.0f),
      FVector(3500.0f, -300.0f, 0.0f),
  };
  for (int32 i = 0; i < TrashPositions.Num(); ++i) {
    CreateBlock(*FString::Printf(TEXT("Trash%d"), i), CylinderMesh,
                TrashPositions[i] + FVector(0, 0, 40.0f),
                FVector(0.3f, 0.3f, 0.8f), TrashColor);
  }

  // Lamp posts
  TArray<FVector> LampPositions = {
      FVector(1000.0f, -350.0f, 0.0f), FVector(1000.0f, 350.0f, 0.0f),
      FVector(2400.0f, -350.0f, 0.0f), FVector(2400.0f, 350.0f, 0.0f),
      FVector(3800.0f, -350.0f, 0.0f), FVector(3800.0f, 350.0f, 0.0f),
  };
  for (int32 i = 0; i < LampPositions.Num(); ++i) {
    // Pole
    CreateBlock(*FString::Printf(TEXT("Lamp%d_Pole"), i), CylinderMesh,
                LampPositions[i] + FVector(0, 0, 150.0f),
                FVector(0.08f, 0.08f, 3.0f), LampColor);
    // Bulb
    CreateBlock(*FString::Printf(TEXT("Lamp%d_Bulb"), i), SphereMesh,
                LampPositions[i] + FVector(0, 0, 320.0f),
                FVector(0.25f, 0.25f, 0.25f), LampBulb);
  }

  // Flower beds near entrance
  TArray<FVector> FlowerPositions = {
      FVector(700.0f, -150.0f, 0.0f),
      FVector(700.0f, 150.0f, 0.0f),
      FVector(900.0f, -150.0f, 0.0f),
      FVector(900.0f, 150.0f, 0.0f),
  };
  for (int32 i = 0; i < FlowerPositions.Num(); ++i) {
    // Soil bed
    CreateBlock(*FString::Printf(TEXT("FlowerBed%d"), i), CylinderMesh,
                FlowerPositions[i] + FVector(0, 0, 5.0f),
                FVector(0.8f, 0.8f, 0.1f), SoilColor);
    // Flowers (small spheres)
    FLinearColor FC = (i % 2 == 0) ? FlowerRed : FlowerYellow;
    CreateBlock(*FString::Printf(TEXT("Flower%d_A"), i), SphereMesh,
                FlowerPositions[i] + FVector(15, 15, 25.0f),
                FVector(0.15f, 0.15f, 0.15f), FC);
    CreateBlock(*FString::Printf(TEXT("Flower%d_B"), i), SphereMesh,
                FlowerPositions[i] + FVector(-15, 10, 22.0f),
                FVector(0.12f, 0.12f, 0.12f), FC);
    CreateBlock(*FString::Printf(TEXT("Flower%d_C"), i), SphereMesh,
                FlowerPositions[i] + FVector(5, -15, 20.0f),
                FVector(0.13f, 0.13f, 0.13f), FC);
  }
}
