#include "AnimalAIController.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "Data/ZooDataTypes.h"
#include "ZooKeeper.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

AAnimalAIController::AAnimalAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	AnimalBehaviorTree  = nullptr;
	AnimalBlackboard    = nullptr;
	CachedNeedsComponent = nullptr;
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

	// If no behavior tree is manually assigned, try to load from species DataTable.
	if (!AnimalBehaviorTree)
	{
		if (FAnimalSpeciesRow* SpeciesRow = Animal->GetSpeciesData())
		{
			UBehaviorTree* SpeciesBT = SpeciesRow->BehaviorTree.LoadSynchronous();
			if (SpeciesBT)
			{
				AnimalBehaviorTree = SpeciesBT;
				UE_LOG(LogZooKeeper, Log, TEXT("AnimalAIController: loaded BT from species DataTable for '%s'."),
				       *Animal->AnimalName);
			}
		}
	}

	// Initialize the blackboard.
	if (AnimalBlackboard)
	{
		UseBlackboard(AnimalBlackboard, Blackboard);
	}

	// Write initial enclosure reference into the blackboard.
	// Need values are synced by BTService_UpdateNeeds during behavior tree execution.
	if (Blackboard)
	{
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

