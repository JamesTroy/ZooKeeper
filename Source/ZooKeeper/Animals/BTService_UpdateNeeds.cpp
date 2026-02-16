#include "BTService_UpdateNeeds.h"
#include "AnimalBase.h"
#include "AnimalNeedsComponent.h"
#include "Buildings/EnclosureActor.h"
#include "ZooKeeper.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateNeeds::UBTService_UpdateNeeds()
{
	NodeName = TEXT("Update Animal Needs");

	// Default tick interval of 1 second.
	Interval      = 1.0f;
	RandomDeviation = 0.1f;
}

void UBTService_UpdateNeeds::TickNode(UBehaviorTreeComponent& OwnerComp,
                                       uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	AAnimalBase* Animal = Cast<AAnimalBase>(AIController->GetPawn());
	if (!Animal || !Animal->NeedsComponent)
	{
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	UAnimalNeedsComponent* Needs = Animal->NeedsComponent;

	BB->SetValueAsFloat(FName("Hunger"),    Needs->Hunger);
	BB->SetValueAsFloat(FName("Thirst"),    Needs->Thirst);
	BB->SetValueAsFloat(FName("Energy"),    Needs->Energy);
	BB->SetValueAsFloat(FName("Happiness"), Needs->Happiness);
	BB->SetValueAsFloat(FName("Social"),    Needs->Social);

	BB->SetValueAsName(FName("MostUrgentNeed"), Needs->GetMostUrgentNeed());
	BB->SetValueAsBool(FName("IsAnyCritical"),  Needs->IsAnyCritical());

	BB->SetValueAsObject(FName("CurrentEnclosure"), Animal->CurrentEnclosure);
}

FString UBTService_UpdateNeeds::GetStaticDescription() const
{
	return TEXT("Syncs AnimalNeedsComponent values to Blackboard keys every tick interval.");
}
