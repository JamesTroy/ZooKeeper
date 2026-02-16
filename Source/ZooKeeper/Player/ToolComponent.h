#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ToolComponent.generated.h"

/**
 * EZooTool
 *
 * The available tools the player can equip.
 */
UENUM(BlueprintType)
enum class EZooTool : uint8
{
	Hand          UMETA(DisplayName = "Hand"),
	FoodBucket    UMETA(DisplayName = "Food Bucket"),
	BuildTool     UMETA(DisplayName = "Build Tool"),
	Binoculars    UMETA(DisplayName = "Binoculars"),
	Tranquilizer  UMETA(DisplayName = "Tranquilizer")
};

/** Broadcast when the active tool changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolChanged, EZooTool, NewTool, EZooTool, OldTool);

/**
 * UToolComponent
 *
 * Component on ZooKeeperCharacter that manages the currently equipped tool.
 * Handles tool switching via scroll wheel and number keys.
 */
UCLASS(ClassGroup = (Zoo), meta = (BlueprintSpawnableComponent, DisplayName = "Tool Component"))
class ZOOKEEPER_API UToolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UToolComponent();

	// -------------------------------------------------------------------
	//  Tool Management
	// -------------------------------------------------------------------

	/** Sets the active tool directly. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tools")
	void SetActiveTool(EZooTool NewTool);

	/** Returns the currently active tool. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Tools")
	EZooTool GetActiveTool() const { return ActiveTool; }

	/** Cycles to the next tool in the list. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tools")
	void CycleToolForward();

	/** Cycles to the previous tool in the list. */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tools")
	void CycleToolBackward();

	/** Sets the tool by index (0-4 for number keys 1-5). */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Tools")
	void SetToolByIndex(int32 Index);

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Tools")
	FOnToolChanged OnToolChanged;

private:
	/** The currently equipped tool. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Tools", meta = (AllowPrivateAccess = "true"))
	EZooTool ActiveTool;

	/** Ordered list of tools for cycling. */
	static const TArray<EZooTool>& GetToolOrder();
};
