#include "TestInteractableActor.h"
#include "ZooKeeper.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATestInteractableActor::ATestInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create and configure the visible mesh.
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Assign the engine's default cube mesh.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube")
	);
	if (CubeMeshFinder.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMeshFinder.Object);
	}

	// Ensure the mesh is visible and responds to traces.
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// -------------------------------------------------------------------
//  IInteractable Implementation
// -------------------------------------------------------------------

FText ATestInteractableActor::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(TEXT("Press E to interact"));
}

void ATestInteractableActor::OnInteract_Implementation(AActor* Interactor)
{
	const FString InteractorName = Interactor ? Interactor->GetName() : TEXT("Unknown");
	UE_LOG(LogZooKeeper, Log, TEXT("Interacted with test actor [%s] by [%s]"), *GetName(), *InteractorName);
}

bool ATestInteractableActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}
