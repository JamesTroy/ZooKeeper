#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StaffSubsystem.generated.h"

class AEnclosureActor;
class AStaffCharacter;

/**
 * EStaffType
 *
 * The role a staff member fulfills in the zoo.
 */
UENUM(BlueprintType)
enum class EStaffType : uint8
{
	Zookeeper     UMETA(DisplayName = "Zookeeper"),
	Veterinarian  UMETA(DisplayName = "Veterinarian"),
	Janitor       UMETA(DisplayName = "Janitor"),
	Mechanic      UMETA(DisplayName = "Mechanic"),
	Guide         UMETA(DisplayName = "Guide")
};

/**
 * FStaffRecord
 *
 * Data record representing a single staff member employed by the zoo.
 */
USTRUCT(BlueprintType)
struct ZOOKEEPER_API FStaffRecord
{
	GENERATED_BODY()

	/** Unique identifier for this staff member. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zoo|Staff")
	int32 StaffID = INDEX_NONE;

	/** The staff member's display name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	FString Name;

	/** The role this staff member performs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	EStaffType Type = EStaffType::Zookeeper;

	/** Daily salary cost. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0"))
	int32 Salary = 100;

	/** Skill level from 0 (novice) to 1 (expert). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Skill = 0.5f;

	/** The enclosure this staff member is assigned to (may be nullptr). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoo|Staff")
	TObjectPtr<AEnclosureActor> AssignedEnclosure = nullptr;
};

/** Broadcast when a new staff member is hired. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaffHired, int32, StaffID);

/** Broadcast when a staff member is fired. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaffFired, int32, StaffID);

/**
 * UStaffSubsystem
 *
 * World subsystem that manages the zoo's staff roster, including hiring,
 * firing, enclosure assignment, and salary calculations.
 */
UCLASS(meta = (DisplayName = "Staff Subsystem"))
class ZOOKEEPER_API UStaffSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// -------------------------------------------------------------------
	//  Staff Management
	// -------------------------------------------------------------------

	/**
	 * Hires a new staff member of the given type.
	 * @param Type  The role for the new hire.
	 * @param Name  The display name for the new staff member.
	 * @return The unique staff ID assigned, or INDEX_NONE on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	int32 HireStaff(EStaffType Type, FString Name);

	/**
	 * Fires the staff member with the given ID.
	 * @param StaffID  The ID of the staff member to fire.
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void FireStaff(int32 StaffID);

	/**
	 * Assigns a staff member to an enclosure.
	 * @param StaffID    The ID of the staff member.
	 * @param Enclosure  The enclosure to assign to (may be nullptr to unassign).
	 */
	UFUNCTION(BlueprintCallable, Category = "Zoo|Staff")
	void AssignToEnclosure(int32 StaffID, AEnclosureActor* Enclosure);

	// -------------------------------------------------------------------
	//  Character Registration
	// -------------------------------------------------------------------

	/** Registers a staff character with this subsystem. Called from StaffCharacter::BeginPlay(). */
	void RegisterStaff(AStaffCharacter* StaffCharacter);

	/** Unregisters a staff character. Called from StaffCharacter::EndPlay(). */
	void UnregisterStaff(AStaffCharacter* StaffCharacter);

	// -------------------------------------------------------------------
	//  Queries
	// -------------------------------------------------------------------

	/** Returns the total number of employed staff members. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff")
	int32 GetStaffCount() const;

	/** Returns the total daily salary cost across all staff members. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff")
	int32 GetDailySalaryCost() const;

	/** Returns a copy of all staff records for UI display. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff")
	TArray<FStaffRecord> GetAllStaffRecords() const { return StaffRecords; }

	/** Returns the staff record for the given ID, or a default if not found. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zoo|Staff")
	FStaffRecord GetStaffRecord(int32 StaffID) const;

	// -------------------------------------------------------------------
	//  Delegates
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Staff")
	FOnStaffHired OnStaffHired;

	UPROPERTY(BlueprintAssignable, Category = "Zoo|Staff")
	FOnStaffFired OnStaffFired;

private:
	/** All staff records. Indexed by position, but looked up by StaffID. */
	UPROPERTY()
	TArray<FStaffRecord> StaffRecords;

	/** Counter for generating unique staff IDs. */
	int32 NextStaffID = 1;

	/** Finds the array index of a staff record by ID, or INDEX_NONE if not found. */
	int32 FindStaffIndex(int32 StaffID) const;

	/** Returns the default salary for a given staff type. */
	static int32 GetDefaultSalary(EStaffType Type);
};
