#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VisitorSubsystem.generated.h"

/** Broadcast when the number of visitors changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisitorCountChanged, int32, NewCount);

/** Broadcast when the average visitor satisfaction level changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSatisfactionChanged, float, NewSatisfaction);

/**
 * FZooVisitorReport
 *
 * Snapshot of visitor-related statistics for display in the UI.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FZooVisitorReport
{
	GENERATED_BODY()

	/** The number of visitors currently in the zoo. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	int32 CurrentCount = 0;

	/** The maximum visitor capacity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	int32 MaxCapacity = 0;

	/** Average satisfaction rating across all visitors (0-100). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	float AverageSatisfaction = 0.0f;

	/** Estimated number of new visitors that would be attracted right now. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	int32 AttractionScore = 0;
};

/**
 * UVisitorSubsystem
 *
 * World subsystem that manages visitor spawning, despawning, satisfaction
 * tracking, and attraction calculations for the zoo.
 */
class AVisitorCharacter;

UCLASS(meta = (DisplayName = "Visitor Subsystem"))
class ZOOKEEPER_API UVisitorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Registration
	// -------------------------------------------------------------------

	/** Registers a visitor character with the subsystem. Called by VisitorCharacter on BeginPlay. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void RegisterVisitor(AVisitorCharacter* Visitor);

	/** Unregisters a visitor character from the subsystem. Called by VisitorCharacter on EndPlay. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void UnregisterVisitor(AVisitorCharacter* Visitor);

	// -------------------------------------------------------------------
	//  Visitor Management
	// -------------------------------------------------------------------

	/**
	 * Spawns the given number of visitors into the zoo, up to MaxVisitors.
	 * @param Count  The number of visitors to spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void SpawnVisitors(int32 Count);

	/** Removes all visitors from the zoo (e.g. at closing time). */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void DespawnAllVisitors();

	/**
	 * Calculates how many new visitors the zoo could attract based on
	 * current animal variety, enclosure quality, cleanliness, etc.
	 * @return An attraction score representing potential new visitors.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitors")
	int32 CalculateVisitorAttraction() const;

	/** Re-evaluates the average satisfaction across all visitors. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitors")
	void UpdateSatisfaction();

	/** Returns a snapshot report of the current visitor state. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitors")
	FZooVisitorReport GetVisitorReport() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Visitors")
	FOnVisitorCountChanged OnVisitorCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Visitors")
	FOnSatisfactionChanged OnSatisfactionChanged;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** Maximum number of visitors the zoo can hold at once. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitors", meta = (ClampMin = "0"))
	int32 MaxVisitors;

	/** Current number of visitors in the zoo. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	int32 CurrentVisitorCount;

	/** Average satisfaction of all visitors (0-100 scale). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Visitors")
	float AverageSatisfaction;

private:
	/** All visitor characters currently in the zoo. */
	UPROPERTY()
	TArray<TObjectPtr<AVisitorCharacter>> AllVisitorCharacters;
};
