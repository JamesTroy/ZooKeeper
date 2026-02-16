#include "ZooHUD.h"

#include "ZooHUDWidget.h"
#include "AnimalInfoWidget.h"
#include "BuildMenuWidget.h"
#include "FinancePanelWidget.h"
#include "StaffRosterWidget.h"
#include "ResearchTreeWidget.h"
#include "ZooOverviewWidget.h"
#include "Animals/AnimalBase.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "ZooKeeper.h"

void AZooHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Green, TEXT(">>> AZooHUD::BeginPlay RUNNING <<<"));
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooHUD: No owning player controller."));
		return;
	}

	// Create the main HUD widget (always visible, z-order 0).
	MainHUDWidget = CreateWidget<UZooHUDWidget>(PC);
	if (MainHUDWidget)
	{
		MainHUDWidget->AddToViewport(0);
		UE_LOG(LogZooKeeper, Log, TEXT("ZooHUD: MainHUDWidget created and added to viewport."));
	}

	// Create panel widgets (start hidden, z-order 1).
	AnimalInfoPanel = CreateWidget<UAnimalInfoWidget>(PC);
	if (AnimalInfoPanel)
	{
		AnimalInfoPanel->AddToViewport(1);
		AnimalInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	BuildMenuPanel = CreateWidget<UBuildMenuWidget>(PC);
	if (BuildMenuPanel)
	{
		BuildMenuPanel->AddToViewport(1);
		BuildMenuPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	FinancePanel = CreateWidget<UFinancePanelWidget>(PC);
	if (FinancePanel)
	{
		FinancePanel->AddToViewport(1);
		FinancePanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	StaffRosterPanel = CreateWidget<UStaffRosterWidget>(PC);
	if (StaffRosterPanel)
	{
		StaffRosterPanel->AddToViewport(1);
		StaffRosterPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	ResearchTreePanel = CreateWidget<UResearchTreeWidget>(PC);
	if (ResearchTreePanel)
	{
		ResearchTreePanel->AddToViewport(1);
		ResearchTreePanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	ZooOverviewPanel = CreateWidget<UZooOverviewWidget>(PC);
	if (ZooOverviewPanel)
	{
		ZooOverviewPanel->AddToViewport(1);
		ZooOverviewPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooHUD: All widgets created."));
}

void AZooHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MainHUDWidget) { MainHUDWidget->RemoveFromParent(); MainHUDWidget = nullptr; }
	if (AnimalInfoPanel) { AnimalInfoPanel->RemoveFromParent(); AnimalInfoPanel = nullptr; }
	if (BuildMenuPanel) { BuildMenuPanel->RemoveFromParent(); BuildMenuPanel = nullptr; }
	if (FinancePanel) { FinancePanel->RemoveFromParent(); FinancePanel = nullptr; }
	if (StaffRosterPanel) { StaffRosterPanel->RemoveFromParent(); StaffRosterPanel = nullptr; }
	if (ResearchTreePanel) { ResearchTreePanel->RemoveFromParent(); ResearchTreePanel = nullptr; }
	if (ZooOverviewPanel) { ZooOverviewPanel->RemoveFromParent(); ZooOverviewPanel = nullptr; }

	Super::EndPlay(EndPlayReason);
}

void AZooHUD::ShowAnimalInfo(AAnimalBase* Animal)
{
	if (AnimalInfoPanel && Animal)
	{
		AnimalInfoPanel->SetAnimalData(Animal);
		AnimalInfoPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AZooHUD::HideAnimalInfo()
{
	if (AnimalInfoPanel)
	{
		AnimalInfoPanel->ClearAnimalData();
		AnimalInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AZooHUD::ToggleBuildMenu()
{
	TogglePanelVisibility(BuildMenuPanel);
}

void AZooHUD::ToggleFinancePanel()
{
	TogglePanelVisibility(FinancePanel);
}

void AZooHUD::ToggleStaffRoster()
{
	TogglePanelVisibility(StaffRosterPanel);
}

void AZooHUD::ToggleResearchTree()
{
	TogglePanelVisibility(ResearchTreePanel);
}

void AZooHUD::ToggleZooOverview()
{
	TogglePanelVisibility(ZooOverviewPanel);
}

void AZooHUD::TogglePanelVisibility(UUserWidget* Panel)
{
	if (!Panel)
	{
		return;
	}

	if (Panel->GetVisibility() == ESlateVisibility::Collapsed)
	{
		Panel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Panel->SetVisibility(ESlateVisibility::Collapsed);
	}
}
