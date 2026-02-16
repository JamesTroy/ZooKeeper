#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalBreedingComponent.generated.h"

class AAnimalBase;

/** Biological sex of an animal, used for breeding eligibility. */
UENUM(BlueprintType)
enum class EAnimalSex : uint8
{
	Male   UMETA(DisplayName = "Male"),
	Female UMETA(DisplayName = "Female"),
};

/** Broadcast when a pregnant animal gives birth. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBabyBorn, AAnimalBase*, Parent);

/**
 * UAnimalBreedingComponent
 *
 * Manages breeding eligibility, gestation tracking, and birth events for an
 * animal. Attach to any AAnimalBase that should be capable of reproduction.
 */
UCLASS(ClassGroup = (Zoo), meta = (BlueprintSpawnableComponent, DisplayName = "Animal Breeding"))
class ZOOKEEPER_API UAnimalBreedingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimalBreedingComponent();

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	// -------------------------------------------------------------------
	//  Properties
	// -------------------------------------------------------------------

	/** Biological sex of this animal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Breeding")
	EAnimalSex Sex;

	/** Minimum age (in game-days) before this animal can breed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Breeding", meta = (ClampMin = "0.0"))
	float MaturityAge;

	/**
	 * Duration of pregnancy in game-days. Only relevant for females.
	 * After this period elapses the OnBabyBorn delegate fires.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Breeding", meta = (ClampMin = "0.1"))
	float GestationPeriod;

	/** Whether this animal is currently pregnant. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Breeding")
	bool bIsPregnant;

	/** Current progress through gestation (0 = just conceived, GestationPeriod = due). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Breeding")
	float GestationProgress;

	// -------------------------------------------------------------------
	//  Functions
	// -------------------------------------------------------------------

	/**
	 * Returns true if this animal meets all requirements to breed:
	 *  - Not already pregnant
	 *  - Old enough (Age >= MaturityAge)
	 *  - Needs are not critical (overall wellbeing above 0.4)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Breeding")
	bool CanBreed() const;

	/**
	 * Attempts to breed this animal with the given partner.
	 * Breeding succeeds if:
	 *  - Both animals can breed
	 *  - They are of opposite sex
	 *  - They are of the same species
	 * On success, the female becomes pregnant.
	 * @param Partner  The other animal's breeding component.
	 * @return true if breeding was initiated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Breeding")
	bool TryBreed(UAnimalBreedingComponent* Partner);

	/**
	 * Advances gestation by the given delta time (in game-day units).
	 * When gestation completes the OnBabyBorn delegate fires and
	 * pregnancy state is reset.
	 * @param DeltaTime  Elapsed time in game-days.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Breeding")
	void TickGestation(float DeltaTime);

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Breeding")
	FOnBabyBorn OnBabyBorn;
};
