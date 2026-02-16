#include "ResearchTreeWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/PanelWidget.h"
#include "ZooKeeper.h"

void UResearchTreeWidget::RefreshResearchTree()
{
	if (!ResearchNodesPanel)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchTreeWidget: ResearchNodesPanel is not bound."));
		return;
	}

	// Clear existing research node widgets.
	ResearchNodesPanel->ClearChildren();

	// TODO: Query the ResearchSubsystem (not yet implemented) for all research nodes
	// and populate the panel. The Blueprint child widget will handle creating
	// individual research node widgets with name, cost, prerequisites, and status.

	// Update current research display.
	UpdateResearchProgress();

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchTreeWidget: Research tree refreshed."));
}

void UResearchTreeWidget::StartResearchClicked(FName ResearchID)
{
	if (ResearchID.IsNone())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ResearchTreeWidget: Attempted to start research with an empty ID."));
		return;
	}

	// TODO: Call ResearchSubsystem->StartResearch(ResearchID) once the subsystem is implemented.
	// The subsystem will validate prerequisites, check costs via EconomySubsystem,
	// and begin the research timer.

	UE_LOG(LogZooKeeper, Log, TEXT("ResearchTreeWidget: Start research clicked for '%s'."), *ResearchID.ToString());

	// Update the display to reflect the newly started research.
	if (CurrentResearchText)
	{
		CurrentResearchText->SetText(FText::FromName(ResearchID));
	}

	if (ResearchProgressBar)
	{
		ResearchProgressBar->SetPercent(0.0f);
	}
}

void UResearchTreeWidget::UpdateResearchProgress()
{
	// TODO: Query the ResearchSubsystem (not yet implemented) for current research
	// progress and update the display accordingly.

	if (CurrentResearchText)
	{
		// Placeholder until ResearchSubsystem is implemented.
		CurrentResearchText->SetText(FText::FromString(TEXT("No Research Active")));
	}

	if (ResearchProgressBar)
	{
		// Placeholder until ResearchSubsystem is implemented.
		ResearchProgressBar->SetPercent(0.0f);
	}
}
