#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZooGameMode.generated.h"

/**
 * AZooGameMode
 *
 * The primary game mode for the Zoo Keeper game. Configures default classes
 * for pawn, player controller, game state, and HUD. Manages high-level
 * game initialization and starting funds.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Game Mode"))
class ZOOKEEPER_API AZooGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZooGameMode();

	//~ Begin AGameModeBase Interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	//~ End AGameModeBase Interface

	/** The amount of funds the player starts with at the beginning of a new game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Economy", meta = (ClampMin = "0"))
	int32 StartingFunds;

private:
	/** Applies the starting funds to the game state. */
	void InitializeGameEconomy();
};
