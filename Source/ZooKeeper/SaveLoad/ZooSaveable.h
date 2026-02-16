#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZooSaveable.generated.h"

/**
 * UZooSaveable
 *
 * UInterface boilerplate class for the saveable interface.
 * Do not add functions here; use IZooSaveable instead.
 */
UINTERFACE(MinimalAPI, meta = (DisplayName = "Zoo Saveable"))
class UZooSaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IZooSaveable
 *
 * Interface for any actor or object that needs to persist its state
 * across save/load cycles. Implementors serialize their data into
 * an archive and provide a unique identifier for lookup.
 */
class ZOOKEEPER_API IZooSaveable
{
	GENERATED_BODY()

public:
	/**
	 * Serializes or deserializes the object's save-relevant data.
	 * The archive direction determines whether this is a save or load operation.
	 * @param Ar  The archive to read from or write to.
	 */
	virtual void SerializeSaveData(FArchive& Ar) = 0;

	/**
	 * Returns a unique identifier for this saveable object.
	 * Used to match saved data back to the correct actor on load.
	 */
	virtual FGuid GetSaveID() const = 0;
};
