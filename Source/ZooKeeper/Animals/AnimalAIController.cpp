#include "AnimalAIController.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "ZooKeeper.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

AAnimalAIController::AAnimalAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AnimalBehaviorTree  = nullptr;
	AnimalBlackboard    = nullptr;
	CachedNeedsComponent = nullptr;

	BlackboardUpdateInterval = 1.0f;
	BlackboardUpdateTimer    = 0.0f;
}

void AAnimalAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AAnimalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAnimalBase* Animal = Cast<AAnimalBase>(InPawn);
	if (!Animal)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalAIController possessed a non-AnimalBase pawn."));
		return;
	}

	CachedNeedsComponent = Animal->NeedsComponent;

	// Initialize the blackboard.
	if (AnimalBlackboard)
	{
		UseBlackboard(AnimalBlackboard, Blackboard);
	}

	// Immediately write the initial need values.
	if (Blackboard)
	{
		UpdateBlackboardFromNeeds();

		// Set the enclosure reference.
		Blackboard->SetValueAsObject(FName("CurrentEnclosure"), Animal->CurrentEnclosure);
	}

	// Start the behavior tree.
	if (AnimalBehaviorTree)
	{
		RunBehaviorTree(AnimalBehaviorTree);
		UE_LOG(LogZooKeeper, Log, TEXT("AnimalAIController: started behavior tree for '%s'."),
		       *Animal->AnimalName);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalAIController: no behavior tree assigned for '%s'."),
		       *Animal->AnimalName);
	}
}

void AAnimalAIController::OnUnPossess()
{
	CachedNeedsComponent = nullptr;
	Super::OnUnPossess();
}

void AAnimalAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BlackboardUpdateTimer += DeltaTime;
	if (BlackboardUpdateTimer >= BlackboardUpdateInterval)
	{
		BlackboardUpdateTimer = 0.0f;
		UpdateBlackboardFromNeeds();
	}
}

void AAnimalAIController::UpdateBlackboardFromNeeds()
{
	if (!Blackboard || !CachedNeedsComponent)
	{
		return;
	}

	Blackboard->SetValueAsFloat(FName("Hunger"),    CachedNeedsComponent->Hunger);
	Blackboard->SetValueAsFloat(FName("Thirst"),    CachedNeedsComponent->Thirst);
	Blackboard->SetValueAsFloat(FName("Energy"),    CachedNeedsComponent->Energy);
	Blackboard->SetValueAsFloat(FName("Happiness"), CachedNeedsComponent->Happiness);
	Blackboard->SetValueAsFloat(FName("Social"),    CachedNeedsComponent->Social);

	Blackboard->SetValueAsName(FName("MostUrgentNeed"), CachedNeedsComponent->GetMostUrgentNeed());
	Blackboard->SetValueAsBool(FName("IsAnyCritical"),  CachedNeedsComponent->IsAnyCritical());

	// Update enclosure reference from the animal.
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AAnimalBase* Animal = Cast<AAnimalBase>(ControlledPawn))
		{
			Blackboard->SetValueAsObject(FName("CurrentEnclosure"), Animal->CurrentEnclosure);
		}
	}
}
