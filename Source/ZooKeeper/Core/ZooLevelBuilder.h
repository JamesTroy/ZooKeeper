#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZooLevelBuilder.generated.h"

class UStaticMesh;
class UMaterialInstanceDynamic;

/**
 * AZooLevelBuilder
 *
 * Spawned by the game mode to procedurally build a zoo layout
 * using engine basic shapes. Creates entrance, paths, enclosures,
 * trees, benches, and decorative elements.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Level Builder"))
class ZOOKEEPER_API AZooLevelBuilder : public AActor {
  GENERATED_BODY()

public:
  AZooLevelBuilder();

protected:
  virtual void BeginPlay() override;

private:
  // Cached mesh references (loaded in constructor)
  UPROPERTY()
  TObjectPtr<UStaticMesh> CubeMesh;

  UPROPERTY()
  TObjectPtr<UStaticMesh> CylinderMesh;

  UPROPERTY()
  TObjectPtr<UStaticMesh> SphereMesh;

  UPROPERTY()
  TObjectPtr<UStaticMesh> PlaneMesh;

  /** Creates a colored static mesh component attached to this actor. */
  UStaticMeshComponent *
  CreateBlock(const FString &Name, UStaticMesh *Mesh, const FVector &Location,
              const FVector &Scale, const FLinearColor &Color,
              const FRotator &Rotation = FRotator::ZeroRotator);

  // --- Build Methods ---
  void BuildEntrance();
  void BuildPaths();
  void BuildEnclosure(const FVector &Center, const FString &Label,
                      const FLinearColor &FenceColor, float SizeX, float SizeY);
  void BuildEnclosures();
  void BuildTrees();
  void BuildBenches();
  void BuildPond();
  void BuildInfoSigns();
  void BuildDecorations();

  int32 BlockCounter;
};
