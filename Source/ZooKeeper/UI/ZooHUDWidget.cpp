#include "ZooHUDWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Subsystems/TimeSubsystem.h"
#include "Subsystems/EconomySubsystem.h"
#include "Interaction/InteractionComponent.h"
#include "Core/ZooKeeperCharacter.h"
#include "ZooKeeper.h"

void UZooHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: No world available during NativeConstruct."));
		return;
	}

	// Bind to the TimeSubsystem delegates.
	UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>();
	if (TimeSys)
	{
		TimeSubsystem = TimeSys;
		TimeSys->OnTimeOfDayChanged.AddDynamic(this, &UZooHUDWidget::HandleTimeOfDayChanged);
		TimeSys->OnDayChanged.AddDynamic(this, &UZooHUDWidget::HandleDayChanged);

		// Initialize with current values.
		UpdateTimeDisplay(TimeSys->GetFormattedTime());
		UpdateDayDisplay(FText::FromString(FString::Printf(TEXT("Day %d"), TimeSys->CurrentDay)));
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: TimeSubsystem not found."));
	}

	// Bind to the EconomySubsystem delegates.
	UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>();
	if (EconSys)
	{
		EconomySubsystem = EconSys;
		EconSys->OnFundsChanged.AddDynamic(this, &UZooHUDWidget::HandleFundsChanged);

		// Initialize with current balance.
		UpdateFundsDisplay(EconSys->GetBalance());
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooHUDWidget: EconomySubsystem not found."));
	}

	// Hide interaction prompt by default.
	HideInteractionPrompt();
}

void UZooHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Attempt to cache the player's interaction component if not yet cached.
	if (!CachedInteractionComp.IsValid())
	{
		if (const APlayerController* PC = GetOwningPlayer())
		{
			if (const AZooKeeperCharacter* Character = Cast<AZooKeeperCharacter>(PC->GetPawn()))
			{
				CachedInteractionComp = Character->GetInteractionComponent();
			}
		}
	}

	// Update interaction prompt from the player's InteractionComponent.
	if (CachedInteractionComp.IsValid())
	{
		UInteractionComponent* InteractionComp = CachedInteractionComp.Get();
		if (InteractionComp->HasInteractable())
		{
			ShowInteractionPrompt(InteractionComp->GetFocusedInteractionPrompt());
		}
		else
		{
			HideInteractionPrompt();
		}
	}
}

void UZooHUDWidget::UpdateTimeDisplay(FText Time)
{
	if (TimeText)
	{
		TimeText->SetText(Time);
	}
}

void UZooHUDWidget::UpdateDayDisplay(FText Day)
{
	if (DayText)
	{
		DayText->SetText(Day);
	}
}

void UZooHUDWidget::UpdateFundsDisplay(int32 Funds)
{
	if (FundsText)
	{
		FundsText->SetText(FText::FromString(FString::Printf(TEXT("$%d"), Funds)));
	}
}

void UZooHUDWidget::ShowInteractionPrompt(FText Prompt)
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetText(Prompt);
		InteractionPromptText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UZooHUDWidget::HideInteractionPrompt()
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UZooHUDWidget::SetCrosshairVisible(bool bVisible)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UZooHUDWidget::HandleTimeOfDayChanged(float NewTime)
{
	if (TimeSubsystem.IsValid())
	{
		UpdateTimeDisplay(TimeSubsystem->GetFormattedTime());
	}
}

void UZooHUDWidget::HandleDayChanged(int32 NewDay)
{
	UpdateDayDisplay(FText::FromString(FString::Printf(TEXT("Day %d"), NewDay)));
}

void UZooHUDWidget::HandleFundsChanged(int32 NewBalance)
{
	UpdateFundsDisplay(NewBalance);
}
