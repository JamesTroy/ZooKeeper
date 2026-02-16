#include "ZooGameMode.h"

#include "ZooGameState.h"
#include "ZooPlayerController.h"
#include "ZooKeeperCharacter.h"
#include "ZooKeeper.h"

#include "UI/ZooHUD.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

AZooGameMode::AZooGameMode()
{
	// Set default classes
	DefaultPawnClass = AZooKeeperCharacter::StaticClass();
	PlayerControllerClass = AZooPlayerController::StaticClass();
	GameStateClass = AZooGameState::StaticClass();
	HUDClass = AZooHUD::StaticClass();

	// Economy defaults
	StartingFunds = 50000;
}

void AZooGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogZooKeeper, Log, TEXT("ZooGameMode::InitGame - Map: %s | Options: %s"), *MapName, *Options);

	if (!ErrorMessage.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooGameMode::InitGame - ErrorMessage: %s"), *ErrorMessage);
	}
}

void AZooGameMode::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogZooKeeper, Log, TEXT("ZooGameMode::StartPlay - Zoo Keeper game session starting."));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Cyan,
			FString::Printf(TEXT(">>> GameMode HUDClass = %s <<<"), HUDClass ? *HUDClass->GetName() : TEXT("NULL")));
	}

	InitializeGameEconomy();
}

void AZooGameMode::InitializeGameEconomy()
{
	AZooGameState* ZooState = GetGameState<AZooGameState>();
	if (ZooState)
	{
		ZooState->AddFunds(StartingFunds);
		ZooState->SetReputation(2.5f); // Start with a neutral reputation
		UE_LOG(LogZooKeeper, Log, TEXT("ZooGameMode - Economy initialized: StartingFunds=%d, Reputation=2.5"), StartingFunds);
	}
	else
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameMode::InitializeGameEconomy - Failed to get ZooGameState."));
	}
}
