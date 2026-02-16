#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VisitorCharacter.generated.h"

/**
 * EVisitorState
 *
 * Enumerates the possible behavioral states of a visitor in the zoo.
 */
UENUM(BlueprintType)
enum class EVisitorState : uint8
{
	Entering			UMETA(DisplayName = "Entering"),
	WalkingToAttraction	UMETA(DisplayName = "Walking To Attraction"),
	ViewingAnimal		UMETA(DisplayName = "Viewing Animal"),
	BuyingFood			UMETA(DisplayName = "Buying Food"),
	Resting				UMETA(DisplayName = "Resting"),
	Leaving				UMETA(DisplayName = "Leaving")
};

/**
 * AVisitorCharacter
 *
 * Represents a visitor navigating the zoo. Tracks satisfaction, spending
 * money, and time in the zoo. Registers/unregisters with the VisitorSubsystem
 * on BeginPlay/EndPlay respectively.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Visitor Character"))
class ZOOKEEPER_API AVisitorCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVisitorCharacter();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	// -------------------------------------------------------------------
	//  Actions
	// -------------------------------------------------------------------

	/**
	 * Adjusts the visitor's satisfaction by the given delta, clamped to [0, 1].
	 * @param Delta  The amount to add (positive) or subtract (negative).
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitor")
	void UpdateSatisfaction(float Delta);

	/**
	 * Spends the given amount of money if the visitor has enough.
	 * @param Amount  The amount to spend (must be positive).
	 * @return true if the visitor had enough money and the purchase succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Visitor")
	bool SpendMoney(int32 Amount);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns the current satisfaction level in the range [0, 1]. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitor")
	float GetSatisfaction() const;

	/** Returns true if the visitor has exceeded their maximum time or has no money and low satisfaction. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Visitor")
	bool ShouldLeave() const;

	// -------------------------------------------------------------------
	//  State
	// -------------------------------------------------------------------

	/** Current satisfaction level. 0 = miserable, 1 = ecstatic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitor", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Satisfaction;

	/** Amount of money the visitor is willing to spend. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitor", meta = (ClampMin = "0"))
	int32 MoneyToSpend;

	/** Admission fee paid on entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitor", meta = (ClampMin = "0"))
	int32 AdmissionFee;

	/** Elapsed time (in seconds) this visitor has been in the zoo. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Visitor")
	float TimeInZoo;

	/** Maximum time (in seconds) this visitor will stay before wanting to leave. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitor", meta = (ClampMin = "0.0"))
	float MaxTimeInZoo;

	/** The visitor's current behavioral state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Visitor")
	EVisitorState CurrentState;
};
