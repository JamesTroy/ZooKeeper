#include "ZooSaveSubsystem.h"
#include "ZooSaveGame.h"
#include "Subsystems/TimeSubsystem.h"
#include "Subsystems/EconomySubsystem.h"
#include "Subsystems/AnimalManagerSubsystem.h"
#include "Subsystems/StaffSubsystem.h"
#include "Subsystems/ResearchSubsystem.h"
#include "Subsystems/MilestoneSubsystem.h"
#include "Subsystems/WeatherSubsystem.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
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

	SaveGameInstance->ZooName = TEXT("My Zoo");

	// --- Time ---
	if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
	{
		SaveGameInstance->SavedDay = TimeSys->CurrentDay;
		SaveGameInstance->SavedTimeOfDay = TimeSys->CurrentTimeOfDay;
		SaveGameInstance->SavedSeason = TimeSys->CurrentSeason;
	}

	// --- Economy ---
	if (UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>())
	{
		SaveGameInstance->SavedFunds = EconSys->GetBalance();
	}

	// --- Animals ---
	if (UAnimalManagerSubsystem* AnimalMgr = World->GetSubsystem<UAnimalManagerSubsystem>())
	{
		TArray<AAnimalBase*> Animals = AnimalMgr->GetAnimalsInEnclosure(nullptr);
		for (AAnimalBase* Animal : Animals)
		{
			if (!Animal) continue;

			FZooAnimalSaveData AnimalData;
			AnimalData.SpeciesID = Animal->SpeciesID;
			AnimalData.Transform = Animal->GetActorTransform();
			AnimalData.AnimalName = Animal->AnimalName;

			if (Animal->NeedsComponent)
			{
				AnimalData.Hunger = Animal->NeedsComponent->GetNeedValue(FName("Hunger"));
				AnimalData.Thirst = Animal->NeedsComponent->GetNeedValue(FName("Thirst"));
				AnimalData.Energy = Animal->NeedsComponent->GetNeedValue(FName("Energy"));
				AnimalData.Health = Animal->NeedsComponent->GetNeedValue(FName("Health"));
				AnimalData.Happiness = Animal->NeedsComponent->GetNeedValue(FName("Happiness"));
				AnimalData.Social = Animal->NeedsComponent->GetNeedValue(FName("Social"));
			}

			SaveGameInstance->SavedAnimals.Add(AnimalData);
		}
	}

	// --- Staff ---
	if (UStaffSubsystem* StaffSys = World->GetSubsystem<UStaffSubsystem>())
	{
		TArray<FStaffRecord> Records = StaffSys->GetAllStaffRecords();
		for (const FStaffRecord& Record : Records)
		{
			FZooStaffSaveData StaffData;
			StaffData.StaffID = Record.StaffID;
			StaffData.Name = Record.Name;
			StaffData.Type = static_cast<uint8>(Record.Type);
			StaffData.Salary = Record.Salary;
			StaffData.Skill = Record.Skill;
			SaveGameInstance->SavedStaff.Add(StaffData);
		}
	}

	// --- Research ---
	if (UResearchSubsystem* ResearchSys = World->GetSubsystem<UResearchSubsystem>())
	{
		SaveGameInstance->CurrentResearchID = ResearchSys->GetCurrentResearchID();
		SaveGameInstance->CurrentResearchProgress = ResearchSys->GetCurrentResearchProgress();
	}

	// --- Milestones ---
	if (UMilestoneSubsystem* MilestoneSys = World->GetSubsystem<UMilestoneSubsystem>())
	{
		SaveGameInstance->AchievedMilestones = MilestoneSys->GetAchievedMilestones();
	}

	// --- Weather ---
	if (UWeatherSubsystem* WeatherSys = World->GetSubsystem<UWeatherSubsystem>())
	{
		SaveGameInstance->SavedWeatherState = static_cast<uint8>(WeatherSys->CurrentWeather);
	}

	// --- Player ---
	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			SaveGameInstance->PlayerTransform = Pawn->GetActorTransform();
		}
	}

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
	UGameInstance* GI = GetGameInstance();
	UWorld* World = GI ? GI->GetWorld() : nullptr;

	if (World)
	{
		// --- Time ---
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->CurrentDay = ZooSave->SavedDay;
			TimeSys->CurrentTimeOfDay = ZooSave->SavedTimeOfDay;
			TimeSys->CurrentSeason = ZooSave->SavedSeason;
		}

		// --- Economy ---
		if (UEconomySubsystem* EconSys = World->GetSubsystem<UEconomySubsystem>())
		{
			EconSys->CurrentFunds = ZooSave->SavedFunds;
			EconSys->OnFundsChanged.Broadcast(EconSys->CurrentFunds);
		}

		// --- Weather ---
		if (UWeatherSubsystem* WeatherSys = World->GetSubsystem<UWeatherSubsystem>())
		{
			WeatherSys->ForceWeather(static_cast<EWeatherState>(ZooSave->SavedWeatherState));
		}

		// --- Player ---
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->SetActorTransform(ZooSave->PlayerTransform);
			}
		}
	}

	UE_LOG(LogZooKeeper, Log, TEXT("ZooSaveSubsystem - Game loaded from slot '%s': Zoo='%s', Day=%d, Time=%.2f, Funds=%d, Animals=%d, Staff=%d"),
		*SlotName, *ZooSave->ZooName, ZooSave->SavedDay, ZooSave->SavedTimeOfDay, ZooSave->SavedFunds,
		ZooSave->SavedAnimals.Num(), ZooSave->SavedStaff.Num());

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
