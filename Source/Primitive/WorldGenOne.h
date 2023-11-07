// Copyright 2021 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "FastNoise/VoxelFastNoise.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include <vector>
#include "WorldGenOne.generated.h"

UCLASS(Blueprintable)
class UWorldGenOne : public UVoxelGenerator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
		int32 MaxFoliageInstances = 400000; // Non-zero value sets the maximum total foliage instances to actually be added to the map

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing")
		int32 MaxFoliageRange = 0; // Non-zero value sets the maximum range from (0, 0) coords where to place foliage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float TerrainHeight = 8000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float WaterVariation = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float WaterVariationFreq = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float PolarTemperature = -55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float EquatorTemperature = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float TemperatureVariation = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float TemperatureVariationFreq = 0.0005f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float TemperatureHeightCoeff = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float PolarMoisture = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float EquatorMoisture = 98.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float MoistureVariation = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate")
		float MoistureVariationFreq = 0.01f;

	//~ Begin UWorldGenOne Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UWorldGenOne Interface
};

class Curve
{
public:
	float GetValueAt(float inValue) const;

	std::vector<std::pair<float, float>>	Points;
};

class FWorldGenOneInstance : public TVoxelGeneratorInstanceHelper<FWorldGenOneInstance, UWorldGenOne>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FWorldGenOneInstance, UWorldGenOne>;

	explicit FWorldGenOneInstance(const UWorldGenOne& MyGenerator);

	//~ Begin FWorldGenOneInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override;

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	//~ End FWorldGenOneInstance Interface

	float GetTerrainHeight(v_flt X, v_flt Y, v_flt Z) const;
	float GetNearLowestTerrainHeight(v_flt X, v_flt Y) const;
	float GetMoisture(v_flt X, v_flt Y, v_flt Z) const;
	float GetTemperature(v_flt X, v_flt Y, v_flt Z) const;
	float GetLatitude(v_flt Y) const;
	int32 GetFoilageType(v_flt X, v_flt Y, v_flt Z, FRotator &outRotation, FVector &outScale, FVector& outOffset) const;

	static TSharedPtr<FWorldGenOneInstance> sGeneratorInstance; // ???? DIRTY!

	void GenerateFoilage(TArray<UInstancedStaticMeshComponent*>& components);

	float VoxelSize = 20.0f;
	int32 WorldSize = 1;

	const int32 MaxFoliageInstances;
	const int32 MaxFoliageRange;

	const float TerrainHeight;
	const float WaterVariation, WaterVariationFreq;
	const float TemperatureVariation, PolarTemperature, EquatorTemperature, TemperatureVariationFreq, TemperatureHeightCoeff;
	const float MoistureVariation, PolarMoisture, EquatorMoisture, MoistureVariationFreq;
	const int32 Seed;

	// Foliage Component IDs - below are defaults, but eventually these are determined dynamically based on component name
	// ???? TODO: Make more parametric and configurable
	int32 ID_None = -1;
	int32 ID_Grass1 = 0;
	int32 ID_Grass2 = 1;
	int32 ID_Rock1 = 2;
	int32 ID_Tree1 = 3; // Mid-sized tree
	int32 ID_Tree2 = 4; // Big tree
	int32 ID_Tree3 = 5; // Small tree

	Curve CurveMountains;
	Curve CurveIslands;
	Curve CurveLowlands;
	Curve CurveSwamps;

private:
	FVoxelFastNoise Noise, WaterNoise, TemperatureNoise, MoistureNoise;
};
