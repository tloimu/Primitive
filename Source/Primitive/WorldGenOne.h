// Copyright 2021 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "FastNoise/VoxelFastNoise.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "WorldGenOne.generated.h"

UCLASS(Blueprintable)
class UWorldGenOne : public UVoxelGenerator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
		int32 MaxFoliageInstances = 400000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
		int32 MaxFoliageRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
		int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Terrain")
		float TerrainHeight = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Water")
		float WaterLevel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Water")
		float WaterVariation = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Water")
		float WaterVariationFreq = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Temperature")
		float PolarTemperature = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Temperature")
		float EquatorTemperature = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Temperature")
		float TemperatureVariation = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Temperature")
		float TemperatureVariationFreq = 0.002f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Temperature")
		float TemperatureHeightCoeff = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Moisture")
		float PolarMoisture = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Moisture")
		float EquatorMoisture = 98.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Moisture")
		float MoistureVariation = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Moisture")
		float MoistureVariationFreq = 0.01f;

	//~ Begin UWorldGenOne Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UWorldGenOne Interface
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

	int32 GetWorldSize() const { return WorldSize; }
	float GetWaterLevel() const { return WaterLevel; }

	static FWorldGenOneInstance* sGeneratorInstance; // ???? DIRTY!

	void GenerateFoilage(AInstancedFoliageActor& foliageActor);

	float VoxelSize = 20.0f;
	int32 WorldSize = 1;

	const int32 MaxFoliageInstances;
	const int32 MaxFoliageRange;

	const float TerrainHeight;
	const float WaterLevel, WaterVariation, WaterVariationFreq;
	const float TemperatureVariation, PolarTemperature, EquatorTemperature, TemperatureVariationFreq, TemperatureHeightCoeff;
	const float MoistureVariation, PolarMoisture, EquatorMoisture, MoistureVariationFreq;
	const int32 Seed;
private:
	FVoxelFastNoise Noise, WaterNoise, TemperatureNoise, MoistureNoise;
};
