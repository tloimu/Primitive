// Copyright 2021 Phyronnaz

#include "WorldGenOne.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"

TVoxelSharedRef<FVoxelGeneratorInstance> UWorldGenOne::GetInstance()
{
	return MakeVoxelShared<FWorldGenOneInstance>(*this);
}

///////////////////////////////////////////////////////////////////////////////
/*
		float LatitudeTemperatureCoeff = 0.1f;
		float EquatorTemperature = 32.0f;
		float TemperatureNoiseHeight = 10.0f;
		float TemperatureNoiseFreq = 1.0f;
		int32 WorldSize = 4096;


		Materials:
		
		0 - grass rock + brown mud rocks
		1 - water
		2 - concrete floor
		3 - coral mud
		4 - medieval blocks
		5 - pavement
		6 - rock
		7 - rock ground
		8 - snow
		9 - grass rock
		10- brown mud rocks
*/

FWorldGenOneInstance::FWorldGenOneInstance(const UWorldGenOne& MyGenerator)
	: Super(&MyGenerator)
	, WorldSize(MyGenerator.WorldSize)
	, TerrainHeight(MyGenerator.TerrainHeight)
	, Seed(MyGenerator.Seed)
	, WaterLevel(MyGenerator.WaterLevel)
	, WaterVariation(MyGenerator.WaterVariation)
	, WaterVariationFreq(MyGenerator.WaterVariationFreq)
	, PolarTemperature(MyGenerator.PolarTemperature)
	, EquatorTemperature(MyGenerator.EquatorTemperature)
	, TemperatureHeightCoeff(MyGenerator.TemperatureHeightCoeff)
	, TemperatureVariation(MyGenerator.TemperatureVariation)
	, TemperatureVariationFreq(MyGenerator.TemperatureVariationFreq)
	, PolarMoisture(MyGenerator.PolarMoisture)
	, EquatorMoisture(MyGenerator.EquatorMoisture)
	, MoistureVariation(MyGenerator.MoistureVariation)
	, MoistureVariationFreq(MyGenerator.MoistureVariationFreq)
{
}

void FWorldGenOneInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	Noise.SetSeed(Seed);
	WaterNoise.SetSeed(Seed + 10);
	TemperatureNoise.SetSeed(Seed + 100);
	MoistureNoise.SetSeed(Seed + 500);
}

float FWorldGenOneInstance::GetTerrainHeight(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	float RoughnessFromMoisture = FMath::Clamp(1.0f - GetMoisture(X, Y, Z, LOD, Items), 0.5f, 1.0f);
	return TerrainHeight * (Noise.GetPerlin_2D(X, Y, 0.001f)
		+ RoughnessFromMoisture * Noise.GetPerlin_2D(X, Y, 0.01f) / 10.0f
		+ RoughnessFromMoisture * Noise.GetPerlin_2D(X, Y, 0.003f) * Noise.GetPerlin_2D(X, Y, 0.1f) / 100.0f);
}

float FWorldGenOneInstance::GetMoisture(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	float Klat = GetLatitude(Y);

	float Mnoise = (MoistureNoise.GetPerlin_2D(X, Y, MoistureVariationFreq)) * MoistureVariation;
	float Mraw = EquatorMoisture - (Klat * (EquatorMoisture - PolarMoisture)) + Mnoise - TerrainHeight * 0.01;
	float M = FMath::Clamp(Mraw, 0.02f, 0.99f);
	return M;
}

float FWorldGenOneInstance::GetLatitude(v_flt Y) const
{
	return (2 * abs(Y) / WorldSize);
}


v_flt FWorldGenOneInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	float Height = GetTerrainHeight(X, Y, Z, LOD, Items);

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// Value positive when Z > Height, and negative Z < Height
	float Value = Z - Height;

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5;

	return Value;
}

FVoxelMaterial FWorldGenOneInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);

	float Height = GetTerrainHeight(X, Y, Z, LOD, Items);
	float Klat = GetLatitude(Y);

	float Tnoise = (TemperatureNoise.GetPerlin_2D(X, Y, TemperatureVariationFreq) - 0.5f)* TemperatureVariation;
	float T = EquatorTemperature - (Klat * (EquatorTemperature - PolarTemperature)) + Tnoise - TemperatureHeightCoeff * Z;

	float M = GetMoisture(X, Y, Z, LOD, Items);

	// ???? TODO: Add moisture's effects to snow thickness and vegetation

	float SnowThickness = T < 0.0f ? -T * 1.0f : 0.0f;

	if (SnowThickness > 0.0f && Z > Height - SnowThickness)
	{
		float snowRatio = FMath::Clamp(SnowThickness / 1.0f, 0.0f, 1.0f);
		Builder.AddMultiIndex(9, snowRatio);
		Builder.AddMultiIndex(8, 1.0f - snowRatio);
	}
	else if (T < 10.0f)
	{
		float grassRatio = FMath::Clamp(T / 10.0f, 0.0f, 1.0f);
		Builder.AddMultiIndex(1, grassRatio);
		Builder.AddMultiIndex(8, 1.0f - grassRatio);
	}
	else
	{
		Builder.AddMultiIndex(1, 1.0f);
	}

	return Builder.Build();
}

TVoxelRange<v_flt> FWorldGenOneInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	return TVoxelRange<v_flt>::Infinite();

	// Example for the GetValueImpl above

	// Noise is between -1 and 1
	const TVoxelRange<v_flt> Height = TVoxelRange<v_flt>(-1, 1) * TerrainHeight;

	// Z can go from min to max
	TVoxelRange<v_flt> Value = TVoxelRange<v_flt>(Bounds.Min.Z, Bounds.Max.Z) - Height;

	Value /= 5;

	return Value;
}

FVector FWorldGenOneInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}
