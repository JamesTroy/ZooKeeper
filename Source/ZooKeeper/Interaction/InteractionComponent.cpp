#include "InteractionComponent.h"
#include "InteractableInterface.h"
#include "ZooKeeper.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UInteractionComponent::UInteractionComponent()
	: InteractionRange(300.0f)
	, TraceRadius(30.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f; // every frame
}

// -------------------------------------------------------------------
//  UActorComponent Interface
// -------------------------------------------------------------------

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformInteractionTrace();
}

// -------------------------------------------------------------------
//  Interaction API
// -------------------------------------------------------------------

void UInteractionComponent::TryInteract()
{
	AActor* Target = FocusedActor.Get();
	if (!Target)
	{
		return;
	}

	if (!IInteractable::Execute_CanInteract(Target, GetOwner()))
	{
		return;
	}

	IInteractable::Execute_OnInteract(Target, GetOwner());
}

FText UInteractionComponent::GetFocusedInteractionPrompt() const
{
	AActor* Target = FocusedActor.Get();
	if (!Target)
	{
		return FText::GetEmpty();
	}

	return IInteractable::Execute_GetInteractionPrompt(Target);
}

bool UInteractionComponent::HasInteractable() const
{
	return FocusedActor.IsValid();
}

// -------------------------------------------------------------------
//  Internal
// -------------------------------------------------------------------

void UInteractionComponent::PerformInteractionTrace()
{
	// Resolve the owning player controller.
	const APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		SetFocusedActor(nullptr);
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController());
	if (!PC || !PC->PlayerCameraManager)
	{
		SetFocusedActor(nullptr);
		return;
	}

	// Compute trace start / end from the camera.
	const FVector TraceStart = PC->PlayerCameraManager->GetCameraLocation();
	const FVector TraceDirection = PC->PlayerCameraManager->GetCameraRotation().Vector();
	const FVector TraceEnd = TraceStart + TraceDirection * InteractionRange;

	// Configure the trace query.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	FHitResult HitResult;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);

	if (!bHit || !HitResult.GetActor())
	{
		SetFocusedActor(nullptr);
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	// Check if the hit actor implements IInteractable.
	if (!HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		SetFocusedActor(nullptr);
		return;
	}

	// Check if interaction is currently allowed.
	if (!IInteractable::Execute_CanInteract(HitActor, GetOwner()))
	{
		SetFocusedActor(nullptr);
		return;
	}

	SetFocusedActor(HitActor);
}

void UInteractionComponent::SetFocusedActor(AActor* NewActor)
{
	AActor* PreviousActor = FocusedActor.Get();
	if (PreviousActor == NewActor)
	{
		return;
	}

	FocusedActor = NewActor;
	OnInteractableFocusChanged.Broadcast(NewActor);
}
