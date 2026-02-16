#include "ZooLevelBuilder.h"

#include "ZooKeeper.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

// ============================================================================
//  Zoo Layout Overview  (X = forward from entrance, Y = left/right)
//
//  Entrance Gate            X =  500,  Y = 0
//  Main Avenue              X =  500 → 2500,  Y = 0
//  Central Plaza + Pond     X = 2500,  Y = 0
//  Left/Right Wing Paths    X = 2500,  Y = ±300 → ±1800
//  Corridor Paths           X = 2500 → 8500,  Y = ±1800
//
//  Enclosure rows along corridors:
//    Row A  X = 3500:  Lions (left),     Elephants (right)
//    Row B  X = 5500:  Penguins (left),  Monkeys (right)
//    Row C  X = 7500:  Bears (left),     Giraffes (right)
//
//  Cross paths at each row connect left ↔ right corridors
//  Back plaza + pond at X = 8500
//  Tigers (left) + Reptiles (right) bracket the back area
// ============================================================================

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

  // Cache the basic shape material (supports "Color" parameter)
  static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
      TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
  if (MatFinder.Succeeded()) {
    BaseMaterial = MatFinder.Object;
  }
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

  // Apply color via dynamic material instance from the BasicShapeMaterial
  UMaterialInterface *MatToUse =
      BaseMaterial.Get() ? BaseMaterial.Get() : Comp->GetMaterial(0);
  if (MatToUse) {
    UMaterialInstanceDynamic *DynMat =
        UMaterialInstanceDynamic::Create(MatToUse, this);
    if (DynMat) {
      DynMat->SetVectorParameterValue(TEXT("Color"), Color);
      Comp->SetMaterial(0, DynMat);
    }
  }

  return Comp;
}

// ---------------------------------------------------------------------------
//  Entrance — Gate pillars + arch + sign
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildEntrance() {
  const FLinearColor Stone(0.45f, 0.4f, 0.35f, 1.0f);
  const FLinearColor DarkWood(0.35f, 0.25f, 0.15f, 1.0f);
  const FLinearColor ZooGreen(0.15f, 0.55f, 0.15f, 1.0f);
  const FLinearColor Gold(0.85f, 0.65f, 0.13f, 1.0f);

  // Left pillar
  CreateBlock(TEXT("Gate_PillarL"), CubeMesh, FVector(500.0f, -250.0f, 200.0f),
              FVector(0.8f, 0.8f, 4.0f), Stone);
  // Right pillar
  CreateBlock(TEXT("Gate_PillarR"), CubeMesh, FVector(500.0f, 250.0f, 200.0f),
              FVector(0.8f, 0.8f, 4.0f), Stone);
  // Arch crossbeam
  CreateBlock(TEXT("Gate_Arch"), CubeMesh, FVector(500.0f, 0.0f, 430.0f),
              FVector(0.6f, 6.0f, 0.5f), DarkWood);
  // "ZOO" sign
  CreateBlock(TEXT("Gate_Sign"), CubeMesh, FVector(500.0f, 0.0f, 490.0f),
              FVector(0.3f, 4.0f, 0.8f), ZooGreen);
  // Gold orbs on pillars
  CreateBlock(TEXT("Gate_OrbL"), SphereMesh, FVector(500.0f, -250.0f, 430.0f),
              FVector(0.5f, 0.5f, 0.5f), Gold);
  CreateBlock(TEXT("Gate_OrbR"), SphereMesh, FVector(500.0f, 250.0f, 430.0f),
              FVector(0.5f, 0.5f, 0.5f), Gold);
}

// ---------------------------------------------------------------------------
//  Paths — Main avenue, wing paths, corridors, and cross paths
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildPaths() {
  const FLinearColor PathColor(0.6f, 0.55f, 0.45f, 1.0f);   // sandy stone
  const FLinearColor PlazaColor(0.55f, 0.50f, 0.42f, 1.0f); // slightly darker

  // ── Main Avenue: entrance (X=500) → hub (X=2500) ──
  for (int32 i = 0; i < 8; ++i) {
    float X = 500.0f + (i * 250.0f);
    CreateBlock(*FString::Printf(TEXT("Avenue_%d"), i), CubeMesh,
                FVector(X, 0.0f, 2.0f), FVector(2.5f, 2.5f, 0.04f), PathColor);
  }

  // ── Central Plaza (circular-ish area at X=2500) ──
  for (int32 dx = -1; dx <= 1; ++dx) {
    for (int32 dy = -1; dy <= 1; ++dy) {
      CreateBlock(*FString::Printf(TEXT("Plaza_%d_%d"), dx + 1, dy + 1),
                  CubeMesh, FVector(2500.0f + dx * 250.0f, dy * 250.0f, 2.0f),
                  FVector(2.5f, 2.5f, 0.04f), PlazaColor);
    }
  }

  // ── Left Wing Path: hub (Y=0) → left corridor (Y=-1800) ──
  for (int32 i = 1; i <= 6; ++i) {
    float Y = -i * 300.0f;
    CreateBlock(*FString::Printf(TEXT("WingL_%d"), i), CubeMesh,
                FVector(2500.0f, Y, 2.0f), FVector(2.5f, 2.5f, 0.04f),
                PathColor);
  }

  // ── Right Wing Path: hub (Y=0) → right corridor (Y=+1800) ──
  for (int32 i = 1; i <= 6; ++i) {
    float Y = i * 300.0f;
    CreateBlock(*FString::Printf(TEXT("WingR_%d"), i), CubeMesh,
                FVector(2500.0f, Y, 2.0f), FVector(2.5f, 2.5f, 0.04f),
                PathColor);
  }

  // ── Left Corridor: runs forward (X=2500→8500) at Y=-1800 ──
  for (int32 i = 0; i < 24; ++i) {
    float X = 2500.0f + (i * 250.0f);
    CreateBlock(*FString::Printf(TEXT("CorridorL_%d"), i), CubeMesh,
                FVector(X, -1800.0f, 2.0f), FVector(2.5f, 2.0f, 0.04f),
                PathColor);
  }

  // ── Right Corridor: runs forward (X=2500→8500) at Y=+1800 ──
  for (int32 i = 0; i < 24; ++i) {
    float X = 2500.0f + (i * 250.0f);
    CreateBlock(*FString::Printf(TEXT("CorridorR_%d"), i), CubeMesh,
                FVector(X, 1800.0f, 2.0f), FVector(2.5f, 2.0f, 0.04f),
                PathColor);
  }

  // ── Cross Paths connecting left ↔ right corridors ──
  // One cross path at each enclosure row + at the back
  float CrossX[] = {3500.0f, 5500.0f, 7500.0f, 8500.0f};
  for (int32 c = 0; c < 4; ++c) {
    for (int32 i = 0; i < 14; ++i) {
      float Y = -1800.0f + (i * 275.0f);
      CreateBlock(*FString::Printf(TEXT("Cross%d_%d"), c, i), CubeMesh,
                  FVector(CrossX[c], Y, 2.0f), FVector(2.0f, 2.5f, 0.04f),
                  PathColor);
    }
  }

  // ── Back Plaza at X=8500 ──
  for (int32 dx = -1; dx <= 1; ++dx) {
    for (int32 dy = -1; dy <= 1; ++dy) {
      CreateBlock(*FString::Printf(TEXT("BackPlaza_%d_%d"), dx + 1, dy + 1),
                  CubeMesh, FVector(8500.0f + dx * 250.0f, dy * 250.0f, 2.0f),
                  FVector(2.5f, 2.5f, 0.04f), PlazaColor);
    }
  }
}

// ---------------------------------------------------------------------------
//  Enclosures — Fenced rectangular areas with corner posts + ground
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildEnclosure(const FVector &Center,
                                      const FString &Label,
                                      const FLinearColor &FenceColor,
                                      float SizeX, float SizeY) {
  const float FH = 150.0f; // fence height
  const float FT = 0.15f;  // fence thickness
  const float PS = 0.4f;   // post size

  // Walls (N/S/E/W)
  CreateBlock(*FString::Printf(TEXT("%s_WallN"), *Label), CubeMesh,
              FVector(Center.X + SizeX / 2, Center.Y, FH / 2),
              FVector(FT, SizeY / 100.0f, FH / 100.0f), FenceColor);
  CreateBlock(*FString::Printf(TEXT("%s_WallS"), *Label), CubeMesh,
              FVector(Center.X - SizeX / 2, Center.Y, FH / 2),
              FVector(FT, SizeY / 100.0f, FH / 100.0f), FenceColor);
  CreateBlock(*FString::Printf(TEXT("%s_WallE"), *Label), CubeMesh,
              FVector(Center.X, Center.Y + SizeY / 2, FH / 2),
              FVector(SizeX / 100.0f, FT, FH / 100.0f), FenceColor);
  CreateBlock(*FString::Printf(TEXT("%s_WallW"), *Label), CubeMesh,
              FVector(Center.X, Center.Y - SizeY / 2, FH / 2),
              FVector(SizeX / 100.0f, FT, FH / 100.0f), FenceColor);

  // Corner posts
  const FLinearColor PostCol(0.3f, 0.3f, 0.3f, 1.0f);
  FVector Corners[4] = {
      {Center.X + SizeX / 2, Center.Y + SizeY / 2, FH / 2},
      {Center.X + SizeX / 2, Center.Y - SizeY / 2, FH / 2},
      {Center.X - SizeX / 2, Center.Y + SizeY / 2, FH / 2},
      {Center.X - SizeX / 2, Center.Y - SizeY / 2, FH / 2},
  };
  for (int32 i = 0; i < 4; ++i) {
    CreateBlock(*FString::Printf(TEXT("%s_Post%d"), *Label, i), CylinderMesh,
                Corners[i], FVector(PS, PS, FH / 100.0f), PostCol);
  }

  // Ground (dirt/sand)
  const FLinearColor Dirt(0.55f, 0.45f, 0.30f, 1.0f);
  CreateBlock(*FString::Printf(TEXT("%s_Ground"), *Label), PlaneMesh,
              FVector(Center.X, Center.Y, 3.0f),
              FVector(SizeX / 100.0f, SizeY / 100.0f, 1.0f), Dirt);
}

void AZooLevelBuilder::BuildEnclosures() {
  // Row A — X=3500 (first pair past central hub)
  // Lions: left corridor, offset inward from path
  BuildEnclosure(FVector(3500.0f, -3200.0f, 0.0f), TEXT("Lions"),
                 FLinearColor(0.7f, 0.4f, 0.1f), 1200.0f, 1000.0f);
  // Elephants: right corridor
  BuildEnclosure(FVector(3500.0f, 3200.0f, 0.0f), TEXT("Elephants"),
                 FLinearColor(0.5f, 0.5f, 0.5f), 1400.0f, 1200.0f);

  // Row B — X=5500
  BuildEnclosure(FVector(5500.0f, -3200.0f, 0.0f), TEXT("Penguins"),
                 FLinearColor(0.2f, 0.4f, 0.7f), 1000.0f, 1000.0f);
  BuildEnclosure(FVector(5500.0f, 3200.0f, 0.0f), TEXT("Monkeys"),
                 FLinearColor(0.2f, 0.6f, 0.2f), 1200.0f, 1000.0f);

  // Row C — X=7500
  BuildEnclosure(FVector(7500.0f, -3200.0f, 0.0f), TEXT("Bears"),
                 FLinearColor(0.35f, 0.2f, 0.1f), 1200.0f, 1200.0f);
  BuildEnclosure(FVector(7500.0f, 3200.0f, 0.0f), TEXT("Giraffes"),
                 FLinearColor(0.75f, 0.6f, 0.2f), 1400.0f, 1200.0f);

  // Back row — X=9000 (flanking the back plaza)
  BuildEnclosure(FVector(9000.0f, -3200.0f, 0.0f), TEXT("Tigers"),
                 FLinearColor(0.8f, 0.35f, 0.05f), 1200.0f, 1000.0f);
  BuildEnclosure(FVector(9000.0f, 3200.0f, 0.0f), TEXT("Reptiles"),
                 FLinearColor(0.1f, 0.4f, 0.15f), 1200.0f, 1000.0f);
}

// ---------------------------------------------------------------------------
//  Trees — Trunk (cylinder) + Canopy (sphere)
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildTrees() {
  const FLinearColor Trunk(0.4f, 0.25f, 0.1f, 1.0f);
  const FLinearColor Leaf1(0.1f, 0.5f, 0.1f, 1.0f);
  const FLinearColor Leaf2(0.05f, 0.4f, 0.08f, 1.0f);

  struct FTree {
    FVector Pos;
    float H; // trunk height
    float R; // canopy radius
    FLinearColor C;
  };

  TArray<FTree> Trees = {
      // ── Along main avenue (alternating sides) ──
      {FVector(700.0f, -350.0f, 0), 250, 200, Leaf1},
      {FVector(700.0f, 350.0f, 0), 270, 210, Leaf2},
      {FVector(1200.0f, -350.0f, 0), 230, 190, Leaf2},
      {FVector(1200.0f, 350.0f, 0), 260, 200, Leaf1},
      {FVector(1700.0f, -350.0f, 0), 280, 220, Leaf1},
      {FVector(1700.0f, 350.0f, 0), 250, 200, Leaf2},

      // ── Central plaza corners ──
      {FVector(2200.0f, -500.0f, 0), 300, 250, Leaf1},
      {FVector(2200.0f, 500.0f, 0), 290, 240, Leaf2},
      {FVector(2800.0f, -500.0f, 0), 310, 250, Leaf2},
      {FVector(2800.0f, 500.0f, 0), 300, 240, Leaf1},

      // ── Along left corridor ──
      {FVector(3000.0f, -2200.0f, 0), 260, 210, Leaf1},
      {FVector(4500.0f, -2200.0f, 0), 280, 230, Leaf2},
      {FVector(6000.0f, -2200.0f, 0), 270, 220, Leaf1},
      {FVector(7000.0f, -2200.0f, 0), 290, 240, Leaf2},
      {FVector(8000.0f, -2200.0f, 0), 250, 200, Leaf1},

      // ── Along right corridor ──
      {FVector(3000.0f, 2200.0f, 0), 270, 220, Leaf2},
      {FVector(4500.0f, 2200.0f, 0), 260, 210, Leaf1},
      {FVector(6000.0f, 2200.0f, 0), 280, 230, Leaf2},
      {FVector(7000.0f, 2200.0f, 0), 300, 250, Leaf1},
      {FVector(8000.0f, 2200.0f, 0), 260, 210, Leaf2},

      // ── Cross paths (between enclosures & paths) ──
      {FVector(3500.0f, -800.0f, 0), 240, 200, Leaf1},
      {FVector(3500.0f, 800.0f, 0), 250, 210, Leaf2},
      {FVector(5500.0f, -800.0f, 0), 270, 220, Leaf2},
      {FVector(5500.0f, 800.0f, 0), 260, 210, Leaf1},

      // ── Back plaza ──
      {FVector(8500.0f, -500.0f, 0), 320, 260, Leaf1},
      {FVector(8500.0f, 500.0f, 0), 310, 250, Leaf2},
      {FVector(9200.0f, 0.0f, 0), 330, 270, Leaf1},
  };

  for (int32 i = 0; i < Trees.Num(); ++i) {
    const FTree &T = Trees[i];
    CreateBlock(*FString::Printf(TEXT("Tree%d_Trunk"), i), CylinderMesh,
                FVector(T.Pos.X, T.Pos.Y, T.H / 2.0f),
                FVector(0.4f, 0.4f, T.H / 100.0f), Trunk);
    CreateBlock(*FString::Printf(TEXT("Tree%d_Canopy"), i), SphereMesh,
                FVector(T.Pos.X, T.Pos.Y, T.H + T.R * 0.5f),
                FVector(T.R / 50.0f, T.R / 50.0f, T.R / 60.0f), T.C);
  }
}

// ---------------------------------------------------------------------------
//  Benches — Seat + backrest + legs, placed logically along paths
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildBenches() {
  const FLinearColor Wood(0.5f, 0.35f, 0.2f, 1.0f);
  const FLinearColor Metal(0.3f, 0.3f, 0.3f, 1.0f);

  struct FBench {
    FVector Pos;
    FRotator Rot;
  };

  TArray<FBench> Benches = {
      // Along main avenue (facing the path)
      {FVector(900.0f, -300.0f, 0), FRotator(0, 90, 0)},
      {FVector(900.0f, 300.0f, 0), FRotator(0, -90, 0)},
      {FVector(1500.0f, -300.0f, 0), FRotator(0, 90, 0)},
      {FVector(1500.0f, 300.0f, 0), FRotator(0, -90, 0)},
      // Beside each cross path (facing enclosures)
      {FVector(3500.0f, -1300.0f, 0), FRotator(0, -90, 0)},
      {FVector(3500.0f, 1300.0f, 0), FRotator(0, 90, 0)},
      {FVector(5500.0f, -1300.0f, 0), FRotator(0, -90, 0)},
      {FVector(5500.0f, 1300.0f, 0), FRotator(0, 90, 0)},
      {FVector(7500.0f, -1300.0f, 0), FRotator(0, -90, 0)},
      {FVector(7500.0f, 1300.0f, 0), FRotator(0, 90, 0)},
      // Back plaza
      {FVector(8500.0f, -400.0f, 0), FRotator(0, 0, 0)},
      {FVector(8500.0f, 400.0f, 0), FRotator(0, 0, 0)},
  };

  for (int32 i = 0; i < Benches.Num(); ++i) {
    const FBench &B = Benches[i];
    CreateBlock(*FString::Printf(TEXT("Bench%d_Seat"), i), CubeMesh,
                B.Pos + FVector(0, 0, 45), FVector(1.2f, 0.5f, 0.08f), Wood,
                B.Rot);
    CreateBlock(*FString::Printf(TEXT("Bench%d_Back"), i), CubeMesh,
                B.Pos + FVector(-20, 0, 80), FVector(0.08f, 0.5f, 0.6f), Wood,
                B.Rot);
    CreateBlock(*FString::Printf(TEXT("Bench%d_LegL"), i), CubeMesh,
                B.Pos + FVector(0, -20, 22), FVector(0.08f, 0.08f, 0.42f),
                Metal, B.Rot);
    CreateBlock(*FString::Printf(TEXT("Bench%d_LegR"), i), CubeMesh,
                B.Pos + FVector(0, 20, 22), FVector(0.08f, 0.08f, 0.42f), Metal,
                B.Rot);
  }
}

// ---------------------------------------------------------------------------
//  Ponds — Central hub pond + back plaza pond
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildPond() {
  const FLinearColor Water(0.1f, 0.3f, 0.6f, 1.0f);
  const FLinearColor Rock(0.4f, 0.4f, 0.38f, 1.0f);

  // ── Central Plaza Pond ──
  CreateBlock(TEXT("CentralPond"), CylinderMesh, FVector(2500.0f, 0.0f, 1.0f),
              FVector(3.5f, 3.5f, 0.02f), Water);
  for (int32 i = 0; i < 8; ++i) {
    float Rad = FMath::DegreesToRadians(i * 45.0f);
    CreateBlock(*FString::Printf(TEXT("CPondRock_%d"), i), SphereMesh,
                FVector(2500.0f + FMath::Cos(Rad) * 370.0f,
                        FMath::Sin(Rad) * 370.0f, 15.0f),
                FVector(0.45f, 0.45f, 0.22f), Rock);
  }

  // ── Back Plaza Pond (larger) ──
  CreateBlock(TEXT("BackPond"), CylinderMesh, FVector(8500.0f, 0.0f, 1.0f),
              FVector(4.5f, 4.5f, 0.02f), Water);
  for (int32 i = 0; i < 10; ++i) {
    float Rad = FMath::DegreesToRadians(i * 36.0f);
    CreateBlock(*FString::Printf(TEXT("BPondRock_%d"), i), SphereMesh,
                FVector(8500.0f + FMath::Cos(Rad) * 480.0f,
                        FMath::Sin(Rad) * 480.0f, 15.0f),
                FVector(0.4f, 0.4f, 0.2f), Rock);
  }
}

// ---------------------------------------------------------------------------
//  Info Signs — One in front of every enclosure
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildInfoSigns() {
  const FLinearColor Board(0.85f, 0.75f, 0.55f, 1.0f);
  const FLinearColor Post(0.35f, 0.25f, 0.15f, 1.0f);

  // Signs placed at path-side edge of each enclosure
  // Left-side enclosures: sign at Y ≈ -2200 (on the corridor side)
  // Right-side enclosures: sign at Y ≈ +2200
  struct FSign {
    FVector Pos;
    FRotator Rot;
  };
  TArray<FSign> Signs = {
      {FVector(3500.0f, -2200.0f, 0), FRotator(0, 90, 0)}, // Lions
      {FVector(3500.0f, 2200.0f, 0), FRotator(0, -90, 0)}, // Elephants
      {FVector(5500.0f, -2200.0f, 0), FRotator(0, 90, 0)}, // Penguins
      {FVector(5500.0f, 2200.0f, 0), FRotator(0, -90, 0)}, // Monkeys
      {FVector(7500.0f, -2200.0f, 0), FRotator(0, 90, 0)}, // Bears
      {FVector(7500.0f, 2200.0f, 0), FRotator(0, -90, 0)}, // Giraffes
      {FVector(9000.0f, -2200.0f, 0), FRotator(0, 90, 0)}, // Tigers
      {FVector(9000.0f, 2200.0f, 0), FRotator(0, -90, 0)}, // Reptiles
  };

  for (int32 i = 0; i < Signs.Num(); ++i) {
    const FSign &S = Signs[i];
    CreateBlock(*FString::Printf(TEXT("Sign%d_Post"), i), CylinderMesh,
                S.Pos + FVector(0, 0, 60), FVector(0.1f, 0.1f, 1.2f), Post,
                S.Rot);
    CreateBlock(*FString::Printf(TEXT("Sign%d_Board"), i), CubeMesh,
                S.Pos + FVector(0, 0, 130), FVector(0.05f, 0.8f, 0.5f), Board,
                S.Rot);
  }
}

// ---------------------------------------------------------------------------
//  Decorations — Lamp posts, trash cans, flower beds
// ---------------------------------------------------------------------------
void AZooLevelBuilder::BuildDecorations() {
  const FLinearColor LampMetal(0.2f, 0.2f, 0.2f, 1.0f);
  const FLinearColor LampBulb(1.0f, 0.95f, 0.7f, 1.0f);
  const FLinearColor TrashCol(0.3f, 0.35f, 0.3f, 1.0f);
  const FLinearColor Soil(0.3f, 0.2f, 0.1f, 1.0f);
  const FLinearColor Red(0.7f, 0.1f, 0.1f, 1.0f);
  const FLinearColor Yellow(0.9f, 0.8f, 0.1f, 1.0f);

  // ── Lamp Posts — evenly along main avenue & corridors ──
  TArray<FVector> Lamps = {
      // Main avenue
      FVector(700.0f, -280.0f, 0),
      FVector(700.0f, 280.0f, 0),
      FVector(1400.0f, -280.0f, 0),
      FVector(1400.0f, 280.0f, 0),
      FVector(2100.0f, -280.0f, 0),
      FVector(2100.0f, 280.0f, 0),
      // Left corridor
      FVector(3200.0f, -1500.0f, 0),
      FVector(4800.0f, -1500.0f, 0),
      FVector(6300.0f, -1500.0f, 0),
      FVector(7800.0f, -1500.0f, 0),
      // Right corridor
      FVector(3200.0f, 1500.0f, 0),
      FVector(4800.0f, 1500.0f, 0),
      FVector(6300.0f, 1500.0f, 0),
      FVector(7800.0f, 1500.0f, 0),
  };
  for (int32 i = 0; i < Lamps.Num(); ++i) {
    CreateBlock(*FString::Printf(TEXT("Lamp%d_Pole"), i), CylinderMesh,
                Lamps[i] + FVector(0, 0, 150), FVector(0.08f, 0.08f, 3.0f),
                LampMetal);
    CreateBlock(*FString::Printf(TEXT("Lamp%d_Bulb"), i), SphereMesh,
                Lamps[i] + FVector(0, 0, 320), FVector(0.25f, 0.25f, 0.25f),
                LampBulb);
  }

  // ── Trash Cans — at cross-path / corridor intersections ──
  TArray<FVector> Trash = {
      FVector(3500.0f, -1500.0f, 0), FVector(3500.0f, 1500.0f, 0),
      FVector(5500.0f, -1500.0f, 0), FVector(5500.0f, 1500.0f, 0),
      FVector(7500.0f, -1500.0f, 0), FVector(7500.0f, 1500.0f, 0),
      FVector(8500.0f, -300.0f, 0),  FVector(8500.0f, 300.0f, 0),
  };
  for (int32 i = 0; i < Trash.Num(); ++i) {
    CreateBlock(*FString::Printf(TEXT("Trash%d"), i), CylinderMesh,
                Trash[i] + FVector(0, 0, 40), FVector(0.3f, 0.3f, 0.8f),
                TrashCol);
  }

  // ── Flower Beds — at entrance & each plaza ──
  TArray<FVector> Flowers = {
      // Entrance flanks
      FVector(500.0f, -450.0f, 0),
      FVector(500.0f, 450.0f, 0),
      // Central plaza corners
      FVector(2200.0f, -450.0f, 0),
      FVector(2200.0f, 450.0f, 0),
      FVector(2800.0f, -450.0f, 0),
      FVector(2800.0f, 450.0f, 0),
      // Back plaza
      FVector(8200.0f, -450.0f, 0),
      FVector(8200.0f, 450.0f, 0),
  };
  for (int32 i = 0; i < Flowers.Num(); ++i) {
    CreateBlock(*FString::Printf(TEXT("FlowerBed%d"), i), CylinderMesh,
                Flowers[i] + FVector(0, 0, 5), FVector(0.8f, 0.8f, 0.1f), Soil);
    FLinearColor FC = (i % 2 == 0) ? Red : Yellow;
    CreateBlock(*FString::Printf(TEXT("Flower%d_A"), i), SphereMesh,
                Flowers[i] + FVector(15, 15, 25), FVector(0.15f, 0.15f, 0.15f),
                FC);
    CreateBlock(*FString::Printf(TEXT("Flower%d_B"), i), SphereMesh,
                Flowers[i] + FVector(-15, 10, 22), FVector(0.12f, 0.12f, 0.12f),
                FC);
    CreateBlock(*FString::Printf(TEXT("Flower%d_C"), i), SphereMesh,
                Flowers[i] + FVector(5, -15, 20), FVector(0.13f, 0.13f, 0.13f),
                FC);
  }
}
