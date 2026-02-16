#include "ZooGameInstance.h"

#include "ZooKeeper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"

UZooGameInstance::UZooGameInstance()
	: CurrentSaveSlot(TEXT("ZooSave_Default"))
	, SaveUserIndex(0)
{
}

void UZooGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogZooKeeper, Log, TEXT("ZooGameInstance::Init - Game instance initialized. Default save slot: %s"), *CurrentSaveSlot);
}

bool UZooGameInstance::SaveGame()
{
	if (CurrentSaveSlot.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameInstance::SaveGame - CurrentSaveSlot is empty. Cannot save."));
		return false;
	}

	// Create or retrieve the save game object
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass());
	if (!SaveGameObject)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameInstance::SaveGame - Failed to create SaveGame object."));
		return false;
	}

	// TODO: Populate SaveGameObject with zoo state data via a custom UZooSaveGame subclass.

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameObject, CurrentSaveSlot, SaveUserIndex);

	if (bSuccess)
	{
		UE_LOG(LogZooKeeper, Log, TEXT("ZooGameInstance::SaveGame - Successfully saved to slot: %s"), *CurrentSaveSlot);
	}
	else
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameInstance::SaveGame - Failed to save to slot: %s"), *CurrentSaveSlot);
	}

	return bSuccess;
}

bool UZooGameInstance::LoadGame(const FString& SlotName)
{
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameInstance::LoadGame - SlotName is empty. Cannot load."));
		return false;
	}

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SaveUserIndex))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooGameInstance::LoadGame - No save game found in slot: %s"), *SlotName);
		return false;
	}

	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, SaveUserIndex);
	if (!LoadedGame)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooGameInstance::LoadGame - Failed to load save game from slot: %s"), *SlotName);
		return false;
	}

	CurrentSaveSlot = SlotName;

	// TODO: Apply loaded data to the active game state via a custom UZooSaveGame subclass.

	UE_LOG(LogZooKeeper, Log, TEXT("ZooGameInstance::LoadGame - Successfully loaded from slot: %s"), *SlotName);
	return true;
}

void UZooGameInstance::SetCurrentSaveSlot(const FString& SlotName)
{
	if (!SlotName.IsEmpty())
	{
		CurrentSaveSlot = SlotName;
		UE_LOG(LogZooKeeper, Log, TEXT("ZooGameInstance::SetCurrentSaveSlot - Active slot set to: %s"), *CurrentSaveSlot);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooGameInstance::SetCurrentSaveSlot - Attempted to set empty slot name. Ignored."));
	}
}
