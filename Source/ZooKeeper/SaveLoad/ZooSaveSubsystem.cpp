#include "ZooSaveSubsystem.h"
#include "ZooSaveGame.h"
#include "ZooKeeper.h"
#include "Kismet/GameplayStatics.h"

bool UZooSaveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UZooSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CachedSaveGame = nullptr;

	UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem::Initialize"));
}

void UZooSaveSubsystem::Deinitialize()
{
	UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem::Deinitialize"));

	CachedSaveGame = nullptr;

	Super::Deinitialize();
}

void UZooSaveSubsystem::SaveGame(FString SlotName)
{
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooSaveSubsystem::SaveGame - Empty slot name."));
		return;
	}

	UZooSaveGame* SaveGameInstance = Cast<UZooSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UZooSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooSaveSubsystem::SaveGame - Failed to create save game object."));
		return;
	}

	// Gather data from world subsystems.
	// The game instance subsystem does not have direct access to the world,
	// so we go through the game instance -> world context.
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooSaveSubsystem::SaveGame - No game instance available."));
		return;
	}

	UWorld* World = GI->GetWorld();
	if (!World)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooSaveSubsystem::SaveGame - No world available."));
		return;
	}

	// Populate save data from subsystems.
	// TimeSubsystem data
	// EconomySubsystem data
	// These will be populated when subsystem headers are included.
	// For now, we set placeholder values to demonstrate the pipeline.
	SaveGameInstance->SavedFunds = 50000;
	SaveGameInstance->SavedDay = 1;
	SaveGameInstance->SavedTimeOfDay = 6.0f;
	SaveGameInstance->ZooName = TEXT("My Zoo");

	// Serialize to disk
	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, UserIndex))
	{
		CachedSaveGame = SaveGameInstance;

		UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem - Game saved to slot '%s'."), *SlotName);
	}
	else
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooSaveSubsystem::SaveGame - Failed to write save to slot '%s'."), *SlotName);
	}
}

bool UZooSaveSubsystem::LoadGame(FString SlotName)
{
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooSaveSubsystem::LoadGame - Empty slot name."));
		return false;
	}

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooSaveSubsystem::LoadGame - No save exists in slot '%s'."), *SlotName);
		return false;
	}

	USaveGame* LoadedData = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	UZooSaveGame* ZooSave = Cast<UZooSaveGame>(LoadedData);

	if (!ZooSave)
	{
		UE_LOG(LogZooKeeper, Error, TEXT("ZooSaveSubsystem::LoadGame - Failed to load or cast save from slot '%s'."), *SlotName);
		return false;
	}

	CachedSaveGame = ZooSave;

	// Apply loaded data to world subsystems.
	// This will be expanded as subsystems are connected.
	UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem - Game loaded from slot '%s': Zoo='%s', Day=%d, Time=%.2f, Funds=%d"),
		*SlotName, *ZooSave->ZooName, ZooSave->SavedDay, ZooSave->SavedTimeOfDay, ZooSave->SavedFunds);

	return true;
}

void UZooSaveSubsystem::DeleteSave(FString SlotName)
{
	if (SlotName.IsEmpty())
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooSaveSubsystem::DeleteSave - Empty slot name."));
		return;
	}

	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);

		UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem - Deleted save in slot '%s'."), *SlotName);
	}
	else
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("ZooSaveSubsystem::DeleteSave - No save exists in slot '%s'."), *SlotName);
	}
}

bool UZooSaveSubsystem::DoesSaveExist(FString SlotName) const
{
	if (SlotName.IsEmpty())
	{
		return false;
	}

	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

TArray<FString> UZooSaveSubsystem::GetAllSaveSlots() const
{
	TArray<FString> FoundSlots;

	// Scan numbered slot names (ZooSave_0 through ZooSave_N)
	for (int32 i = 0; i < MaxSaveSlots; ++i)
	{
		const FString SlotName = FString::Printf(TEXT("ZooSave_%d"), i);
		if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
		{
			FoundSlots.Add(SlotName);
		}
	}

	// Also check for named saves with common names
	static const TArray<FString> NamedSlots = {
		TEXT("Quicksave"),
		TEXT("Autosave")
	};

	for (const FString& NamedSlot : NamedSlots)
	{
		if (UGameplayStatics::DoesSaveGameExist(NamedSlot, UserIndex))
		{
			FoundSlots.Add(NamedSlot);
		}
	}

	return FoundSlots;
}
