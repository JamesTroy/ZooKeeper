#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DataTable.h"
#include "ResearchSubsystem.generated.h"

/** Broadcast when a research project completes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResearchCompleted, FName, ResearchID);

/** Broadcast when a new research project begins. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResearchStarted, FName, ResearchID);

/**
 * UResearchSubsystem
 *
 * World subsystem that manages the zoo's research tree. Tracks completed research,
 * the currently active research project, and progress toward completion.
 */
UCLASS(meta = (DisplayName = "Research Subsystem"))
class ZOOKEEPER_API UResearchSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Research Management
	// -------------------------------------------------------------------

	/**
	 * Begins researching the given topic. Fails if research is already in progress
	 * or the topic has already been completed.
	 * @param ResearchID  The identifier of the research to start.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void StartResearch(FName ResearchID);

	/**
	 * Advances the current research project by the given delta time.
	 * Should be called each frame (or each game-time tick) while research is active.
	 * @param DeltaTime  Time elapsed in game seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void TickResearch(float DeltaTime);

	/** Cancels the current research project without completing it. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Research")
	void CancelResearch();

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns true if the given research topic has been completed. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Research")
	bool IsResearched(FName ResearchID) const;

	/** Returns a list of all available (not yet completed and not in progress) research topics. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Research")
	TArray<FName> GetAvailableResearch() const;

	/** Returns the ID of the research currently in progress, or NAME_None if idle. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Research")
	FName GetCurrentResearchID() const;

	/** Returns the progress of the current research as a 0-1 value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Research")
	float GetCurrentResearchProgress() const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Research")
	FOnResearchCompleted OnResearchCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Research")
	FOnResearchStarted OnResearchStarted;

	/** The DataTable of FResearchNodeData rows. Assign in editor or load at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Research")
	TObjectPtr<UDataTable> ResearchDataTable;

private:
	/** Set of all completed research IDs. */
	TSet<FName> CompletedResearch;

	/** The research currently being worked on (NAME_None if idle). */
	FName CurrentResearchID;

	/** Progress toward completing the current research (0 to ResearchDuration). */
	float CurrentResearchProgress;

	/** Duration in game seconds required to complete the current research project. */
	float ResearchDuration;

	/** Whether research is currently active. */
	bool bIsResearching;

	/** Cached list of all research IDs loaded from DataTable. */
	TArray<FName> AllResearchTopics;

	/** Loads all research topics from the DataTable (or falls back to hardcoded list). */
	void LoadResearchFromDataTable();
};
