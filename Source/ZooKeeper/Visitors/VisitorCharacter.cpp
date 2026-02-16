#include "VisitorCharacter.h"
#include "Subsystems/VisitorSubsystem.h"
#include "Subsystems/EconomySubsystem.h"
#include "ZooKeeper.h"

AVisitorCharacter::AVisitorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Satisfaction = 0.5f;
	MoneyToSpend = FMath::RandRange(50, 150);
	AdmissionFee = 10;
	TimeInZoo = 0.0f;
	MaxTimeInZoo = FMath::RandRange(300.0f, 600.0f);
	CurrentState = EVisitorState::Entering;
}

void AVisitorCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		UVisitorSubsystem* VisitorSubsystem = World->GetSubsystem<UVisitorSubsystem>();
		if (VisitorSubsystem)
		{
			VisitorSubsystem->RegisterVisitor(this);
			UE_LOG(LogZooKeeper, Log, TEXT("VisitorCharacter [%s] registered with VisitorSubsystem."), *GetName());
		}

		// Pay admission fee to the economy.
		if (AdmissionFee > 0)
		{
			if (UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>())
			{
				EconSys->AddIncome(AdmissionFee, TEXT("Visitor admission"));
			}
			SpendMoney(AdmissionFee);
		}
	}
}

void AVisitorCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UVisitorSubsystem* VisitorSubsystem = World->GetSubsystem<UVisitorSubsystem>();
		if (VisitorSubsystem)
		{
			VisitorSubsystem->UnregisterVisitor(this);
			UE_LOG(LogZooKeeper, Log, TEXT("VisitorCharacter [%s] unregistered from VisitorSubsystem."), *GetName());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AVisitorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeInZoo += DeltaTime;

	// Gradually reduce satisfaction over time if nothing positive happens
	if (CurrentState != EVisitorState::ViewingAnimal && CurrentState != EVisitorState::BuyingFood)
	{
		const float PassiveDecay = 0.001f * DeltaTime;
		UpdateSatisfaction(-PassiveDecay);
	}
}

void AVisitorCharacter::UpdateSatisfaction(float Delta)
{
	const float OldSatisfaction = Satisfaction;
	Satisfaction = FMath::Clamp(Satisfaction + Delta, 0.0f, 1.0f);

	if (!FMath::IsNearlyEqual(OldSatisfaction, Satisfaction))
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("Visitor [%s] satisfaction changed: %.2f -> %.2f"),
			*GetName(), OldSatisfaction, Satisfaction);
	}
}

bool AVisitorCharacter::SpendMoney(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("Visitor [%s] SpendMoney called with non-positive amount: %d"),
			*GetName(), Amount);
		return false;
	}

	if (MoneyToSpend < Amount)
	{
		UE_LOG(LogZooKeeper, Verbose, TEXT("Visitor [%s] cannot afford %d (has %d)."),
			*GetName(), Amount, MoneyToSpend);
		return false;
	}

	MoneyToSpend -= Amount;
	UE_LOG(LogZooKeeper, Verbose, TEXT("Visitor [%s] spent %d, remaining: %d"),
		*GetName(), Amount, MoneyToSpend);
	return true;
}

float AVisitorCharacter::GetSatisfaction() const
{
	return Satisfaction;
}

bool AVisitorCharacter::ShouldLeave() const
{
	// Leave if time is up
	if (TimeInZoo >= MaxTimeInZoo)
	{
		return true;
	}

	// Leave if out of money and satisfaction is very low
	if (MoneyToSpend <= 0 && Satisfaction < 0.2f)
	{
		return true;
	}

	// Leave if currently in the Leaving state
	if (CurrentState == EVisitorState::Leaving)
	{
		return true;
	}

	return false;
}
