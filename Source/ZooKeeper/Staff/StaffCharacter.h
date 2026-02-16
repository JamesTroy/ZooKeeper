#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Subsystems/StaffSubsystem.h"
#include "StaffCharacter.generated.h"

class AEnclosureActor;

/**
 * AStaffCharacter
 *
 * Represents a staff member working in the zoo. Each staff character has a
 * type, name, skill level, and salary. Staff can be assigned to specific
 * enclosures and perform duties based on their type and skill.
 * Registers/unregisters with the StaffSubsystem on BeginPlay/EndPlay.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Staff Character"))
class ZOOKEEPER_API AStaffCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AStaffCharacter();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	// -------------------------------------------------------------------
	//  Actions
	// -------------------------------------------------------------------

	/**
	 * Assigns this staff member to the given enclosure.
	 * @param Enclosure  The enclosure to assign to (may be nullptr to unassign).
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void AssignToEnclosure(AEnclosureActor* Enclosure);

	/**
	 * Performs the staff member's primary duty based on their type.
	 * Zookeepers feed animals, veterinarians treat sick animals,
	 * mechanics repair buildings, janitors clean paths, guides assist visitors.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void PerformDuty();

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/**
	 * Returns the staff member's work efficiency as a multiplier.
	 * Efficiency is derived from SkillLevel: base 0.5 + (SkillLevel * 0.5),
	 * yielding a range of [0.5, 1.0].
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff")
	float GetEfficiency() const;

	// -------------------------------------------------------------------
	//  Properties
	// -------------------------------------------------------------------

	/** The role this staff member fulfills. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	EStaffType StaffType;

	/** The display name of this staff member. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	FString StaffName;

	/** Skill level ranging from 0 (novice) to 1 (expert). Affects work efficiency. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SkillLevel;

	/** Daily salary cost for this staff member. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0"))
	int32 Salary;

	/** The enclosure this staff member is currently assigned to. May be nullptr. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	TObjectPtr<AEnclosureActor> AssignedEnclosure;
};
