#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZooEnvironmentSetup.generated.h"

class UDirectionalLightComponent;
class USkyAtmosphereComponent;
class USkyLightComponent;
class UExponentialHeightFogComponent;
class UStaticMeshComponent;

/**
 * AZooEnvironmentSetup
 *
 * Spawned by the game mode to provide essential environment elements
 * (sun, sky, fog, and ground plane) so that any empty map is immediately
 * playable without manual editor placement.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Zoo Environment Setup"))
class ZOOKEEPER_API AZooEnvironmentSetup : public AActor
{
	GENERATED_BODY()

public:
	AZooEnvironmentSetup();

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	// ---------------------------------------------------------------
	//  Lighting
	// ---------------------------------------------------------------

	/** Directional light acting as the sun. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
	TObjectPtr<UDirectionalLightComponent> SunLight;

	/** Intensity of the directional light in lux. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Sun", meta = (ClampMin = "0.0"))
	float SunIntensity;

	/** Color temperature of the sun light. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Sun")
	FLinearColor SunColor;

	/** Rotation of the sun (pitch = elevation, yaw = compass direction). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Sun")
	FRotator SunRotation;

	// ---------------------------------------------------------------
	//  Sky
	// ---------------------------------------------------------------

	/** Procedural sky atmosphere with Rayleigh / Mie scattering. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	/** Sky light that captures the atmosphere for ambient fill. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
	TObjectPtr<USkyLightComponent> SkyLight;

	/** Intensity multiplier for the sky light. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Sky", meta = (ClampMin = "0.0"))
	float SkyLightIntensity;

	// ---------------------------------------------------------------
	//  Fog
	// ---------------------------------------------------------------

	/** Exponential height fog for atmospheric depth. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
	TObjectPtr<UExponentialHeightFogComponent> HeightFog;

	/** Fog density (lower = more subtle). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Fog", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogDensity;

	/** Fog inscattering color. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Fog")
	FLinearColor FogColor;

	// ---------------------------------------------------------------
	//  Ground
	// ---------------------------------------------------------------

	/** A large flat plane serving as the ground surface. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoo|Environment")
	TObjectPtr<UStaticMeshComponent> GroundPlane;

	/** Scale of the ground plane (each unit in XY ≈ 100 cm). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Ground", meta = (ClampMin = "1.0"))
	float GroundScale;

	/** Color tint applied to the ground material. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoo|Environment|Ground")
	FLinearColor GroundColor;

private:
	/** Creates a simple dynamic material instance for the ground plane. */
	void ApplyGroundMaterial();
};
