#include "ToolComponent.h"
#include "ZooKeeper/ZooKeeper.h"

UToolComponent::UToolComponent()
	: ActiveTool(EZooTool::Hand)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UToolComponent::SetActiveTool(EZooTool NewTool)
{
	if (NewTool == ActiveTool)
	{
		return;
	}

	const EZooTool OldTool = ActiveTool;
	ActiveTool = NewTool;

	OnToolChanged.Broadcast(NewTool, OldTool);

	UE_LOG(LogZooKeeper, Log, TEXT("ToolComponent: Switched from tool %d to tool %d."),
		static_cast<uint8>(OldTool), static_cast<uint8>(NewTool));
}

void UToolComponent::CycleToolForward()
{
	const TArray<EZooTool>& Order = GetToolOrder();
	const int32 CurrentIndex = Order.IndexOfByKey(ActiveTool);
	const int32 NextIndex = (CurrentIndex + 1) % Order.Num();
	SetActiveTool(Order[NextIndex]);
}

void UToolComponent::CycleToolBackward()
{
	const TArray<EZooTool>& Order = GetToolOrder();
	const int32 CurrentIndex = Order.IndexOfByKey(ActiveTool);
	const int32 PrevIndex = (CurrentIndex - 1 + Order.Num()) % Order.Num();
	SetActiveTool(Order[PrevIndex]);
}

void UToolComponent::SetToolByIndex(int32 Index)
{
	const TArray<EZooTool>& Order = GetToolOrder();
	if (Index >= 0 && Index < Order.Num())
	{
		SetActiveTool(Order[Index]);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ToolComponent: Invalid tool index %d (valid range: 0-%d)."),
			Index, Order.Num() - 1);
	}
}

const TArray<EZooTool>& UToolComponent::GetToolOrder()
{
	static const TArray<EZooTool> ToolOrder = {
		EZooTool::Hand,
		EZooTool::FoodBucket,
		EZooTool::BuildTool,
		EZooTool::Binoculars,
		EZooTool::Tranquilizer
	};
	return ToolOrder;
}
