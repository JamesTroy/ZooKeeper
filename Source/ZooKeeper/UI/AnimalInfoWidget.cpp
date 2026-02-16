#include "AnimalInfoWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
#include "ZooKeeper.h"

void UAnimalInfoWidget::SetAnimalData(AAnimalBase* Animal)
{
	if (!Animal)
	{
		ClearAnimalData();
		return;
	}

	CurrentAnimal = Animal;

	// Populate identity fields.
	if (AnimalNameText)
	{
		AnimalNameText->SetText(FText::FromString(Animal->AnimalName));
	}

	if (SpeciesText)
	{
		SpeciesText->SetText(FText::FromName(Animal->SpeciesID));
	}

	if (AgeText)
	{
		AgeText->SetText(FText::FromString(FString::Printf(TEXT("%d days"), Animal->Age)));
	}

	// Update need bars with initial values.
	UpdateNeedBars();

	UE_LOG(LogZooKeeper, Log, TEXT("AnimalInfoWidget: Displaying data for '%s' (%s)."),
		*Animal->AnimalName, *Animal->SpeciesID.ToString());
}

void UAnimalInfoWidget::UpdateNeedBars()
{
	if (!CurrentAnimal.IsValid())
	{
		return;
	}

	AAnimalBase* Animal = CurrentAnimal.Get();
	UAnimalNeedsComponent* Needs = Animal->NeedsComponent;
	if (!Needs)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalInfoWidget: Animal '%s' has no NeedsComponent."),
			*Animal->AnimalName);
		return;
	}

	if (HungerBar)
	{
		HungerBar->SetPercent(Needs->Hunger);
	}

	if (ThirstBar)
	{
		ThirstBar->SetPercent(Needs->Thirst);
	}

	if (EnergyBar)
	{
		EnergyBar->SetPercent(Needs->Energy);
	}

	if (HealthBar)
	{
		HealthBar->SetPercent(Needs->Health);
	}

	if (HappinessBar)
	{
		HappinessBar->SetPercent(Needs->Happiness);
	}

	if (SocialBar)
	{
		SocialBar->SetPercent(Needs->Social);
	}
}

void UAnimalInfoWidget::ClearAnimalData()
{
	CurrentAnimal.Reset();

	if (AnimalNameText)
	{
		AnimalNameText->SetText(FText::GetEmpty());
	}

	if (SpeciesText)
	{
		SpeciesText->SetText(FText::GetEmpty());
	}

	if (AgeText)
	{
		AgeText->SetText(FText::GetEmpty());
	}

	if (HungerBar)
	{
		HungerBar->SetPercent(0.0f);
	}

	if (ThirstBar)
	{
		ThirstBar->SetPercent(0.0f);
	}

	if (EnergyBar)
	{
		EnergyBar->SetPercent(0.0f);
	}

	if (HealthBar)
	{
		HealthBar->SetPercent(0.0f);
	}

	if (HappinessBar)
	{
		HappinessBar->SetPercent(0.0f);
	}

	if (SocialBar)
	{
		SocialBar->SetPercent(0.0f);
	}
}

void UAnimalInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CurrentAnimal.IsValid())
	{
		UpdateNeedBars();
	}
}
