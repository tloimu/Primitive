// Copyright 2021 Phyronnaz

#include "WorldGenOne.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
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

FWorldGenOneInstance* FWorldGenOneInstance::sGeneratorInstance = nullptr;

FWorldGenOneInstance::FWorldGenOneInstance(const UWorldGenOne& MyGenerator)
	: Super(&MyGenerator)
	, WorldSize(MyGenerator.WorldSize)
	, TerrainHeight(MyGenerator.TerrainHeight)
	, WaterLevel(MyGenerator.WaterLevel)
	, WaterVariation(MyGenerator.WaterVariation)
	, WaterVariationFreq(MyGenerator.WaterVariationFreq)
	, TemperatureVariation(MyGenerator.TemperatureVariation)
	, PolarTemperature(MyGenerator.PolarTemperature)
	, EquatorTemperature(MyGenerator.EquatorTemperature)
	, TemperatureVariationFreq(MyGenerator.TemperatureVariationFreq)
	, TemperatureHeightCoeff(MyGenerator.TemperatureHeightCoeff)
	, MoistureVariation(MyGenerator.MoistureVariation)
	, PolarMoisture(MyGenerator.PolarMoisture)
	, EquatorMoisture(MyGenerator.EquatorMoisture)
	, MoistureVariationFreq(MyGenerator.MoistureVariationFreq)
	, Seed(MyGenerator.Seed)
{
}

void
FWorldGenOneInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	Noise.SetSeed(Seed);
	WaterNoise.SetSeed(Seed + 10);
	TemperatureNoise.SetSeed(Seed + 100);
	MoistureNoise.SetSeed(Seed + 500);

	// ???? DIRTY!
	FWorldGenOneInstance::sGeneratorInstance = this;
}


TVoxelRange<v_flt>
FWorldGenOneInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
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

FVector
FWorldGenOneInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}


float
FWorldGenOneInstance::GetTerrainHeight(v_flt X, v_flt Y, v_flt Z) const
{
	float ErosionFromMoisture = FMath::Clamp(1.0f - GetMoisture(X, Y, Z)/110.0f, 0.4f, 1.0f);
	float flatness = FMath::Clamp(Noise.GetPerlin_2D(X, Y, 0.0002f), 0.3f, 1.0f);
	// float lowlandsErosion = FMath::Clamp(Noise.GetPerlin_2D(X, Y, 0.0004f), 0.3f, 1.0f);

	return (TerrainHeight * flatness) * (
		Noise.GetPerlin_2D(X, Y, 0.000001f) * 4.0f * FMath::Clamp(0.5f, 1.0f, Noise.GetPerlin_2D(X, Y, 0.0001f))
		+ Noise.GetPerlin_2D(X, Y, 0.0001f)
		+ Noise.GetPerlin_2D(X, Y, 0.001f)
		+ ErosionFromMoisture * Noise.GetPerlin_2D(X, Y, 0.01f) / 10.0f
		+ ErosionFromMoisture * Noise.GetPerlin_2D(X, Y, 0.003f) * Noise.GetPerlin_2D(X, Y, 0.1f) / 100.0f);
}

float
FWorldGenOneInstance::GetMoisture(v_flt X, v_flt Y, v_flt Z) const
{
	float Klat = GetLatitude(Y);

	float Mnoise = (MoistureNoise.GetPerlin_2D(X, Y, MoistureVariationFreq)) * MoistureVariation;
	float Mraw = EquatorMoisture - (Klat * (EquatorMoisture - PolarMoisture)) + Mnoise - TerrainHeight * 0.01;
	return Mraw;
}

float
FWorldGenOneInstance::GetTemperature(v_flt X, v_flt Y, v_flt Z) const
{
	float Klat = GetLatitude(Y);

	float Tnoise = (TemperatureNoise.GetPerlin_2D(X, Y, TemperatureVariationFreq) - 0.5f)* TemperatureVariation;
	float T = EquatorTemperature - (Klat * (EquatorTemperature - PolarTemperature)) + Tnoise - TemperatureHeightCoeff * Z;

	return T;
}

float
FWorldGenOneInstance::GetLatitude(v_flt Y) const
{
	return (abs(Y) / WorldSize);
}

const int32 ID_None = -1;
const int32 ID_Grass1 = 0;
const int32 ID_Grass2 = 1;
const int32 ID_Rock1 = 2;
const int32 ID_Tree1 = 3;
const int32 ID_Tree2 = 4;
const int32 ID_Tree3 = 5;

float
FWorldGenOneInstance::GetNearLowestTerrainHeight(v_flt X, v_flt Y) const
{
	float z = GetTerrainHeight(X, Y, 0);
	for (int dx = -1; dx < 2; dx++)
	{
		for (int dy = -1; dy < 2; dy++)
		{
			float nz = GetTerrainHeight(X + dx, Y + dy, 0);
			if (nz < z)
				z = nz;
		}
	}
	return z;
}

v_flt
FWorldGenOneInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	float Height = GetTerrainHeight(X, Y, Z);

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// Value positive when Z > Height, and negative Z < Height
	float Value = Z - Height;

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5;

	return Value;
}

FVoxelMaterial
FWorldGenOneInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);

	float Height = GetTerrainHeight(X, Y, Z);
	float T = GetTemperature(X, Y, Z);

	float M = GetMoisture(X, Y, Z);

	// ???? TODO: Add moisture's effects to snow thickness and vegetation

	float SnowThickness = T < -5.0f ? -(T + 5.0f) * 1.0f : 0.0f;
	const int32 snowMaterial = 9;
	const int32 coldMaterial = 8;
	const int32 temperateMaterial = 1;
	const int32 hotMaterial = M < 30.0f ? 7 : 2;

	if (SnowThickness > 0.0f && Z > Height - SnowThickness)
	{
		float snowRatio = FMath::Clamp(SnowThickness / 1.0f, 0.0f, 1.0f);
		Builder.AddMultiIndex(snowMaterial, snowRatio);
		Builder.AddMultiIndex(coldMaterial, 1.0f - snowRatio);
	}
	else if (T < 10.0f)
	{
		float grassRatio = FMath::Clamp(T / 10.0f, 0.0f, 1.0f);
		Builder.AddMultiIndex(temperateMaterial, grassRatio);
		Builder.AddMultiIndex(coldMaterial, 1.0f - grassRatio);
	}
	else if (T < 25.0f)
	{
		Builder.AddMultiIndex(temperateMaterial, 1.0f);
	}
	else
	{
		float desertRatio = FMath::Clamp((T - 25.0f) / 10.0f, 0.0f, 1.0f);
		Builder.AddMultiIndex(hotMaterial, desertRatio);
		Builder.AddMultiIndex(temperateMaterial, 1.0f - desertRatio);
	}

	return Builder.Build();
}


int32
FWorldGenOneInstance::GetFoilageType(v_flt X, v_flt Y, v_flt Z, FRotator& outRotation, FVector& outScale, FVector& outOffset) const
{
	int32 x = (int32)X;
	int32 y = (int32)Y;

	auto T = GetTemperature(X, Y, Z);
	auto M = GetMoisture(X, Y, Z);
	if (T < -10.0f || M < 5.0f) // Too cold an/or dry
		return ID_None;

	int32 type = ID_None;
	outOffset.Z = 0.0f;
	outRotation.Pitch = 0.0f;
	outRotation.Roll = 0.0f;
	outRotation.Yaw = 0.0f;
	float z = 0.0f;

	outOffset.X = FMath::RandHelper(20 * VoxelSize) - 10 * VoxelSize;
	outOffset.Y = FMath::RandHelper(20 * VoxelSize) - 10 * VoxelSize;
	X += outOffset.X / VoxelSize;
	Y += outOffset.Y / VoxelSize;
	auto rocks = (WaterNoise.GetPerlin_2D(X, Y, 0.001f) + WaterNoise.GetPerlin_2D(X, Y, 0.02f));
	auto trees = (TemperatureNoise.GetPerlin_2D(X, Y, 0.001f) + TemperatureNoise.GetPerlin_2D(X, Y, 0.02f));

	if (rocks > 0.5f && (x % 20 == 0 && y % 20 == 0))
	{
		if (T > -15.0f && WaterNoise.GetPerlin_2D(X, Y, 10.0f) * 3.5f < rocks)
		{
			z = GetNearLowestTerrainHeight(X, Y);
			type = ID_Rock1;
			outOffset.Z = -FMath::RandHelper(200);
			outRotation.Pitch = FMath::RandHelper(360);
			outRotation.Roll = FMath::RandHelper(360);
		}
	}
	else if (trees > 0.0f && (x % 30 == 0 && y % 30 == 0))
	{
		if (TemperatureNoise.GetPerlin_2D(X, Y, 10.0f) * 2.0f < trees)
		{
			z = GetNearLowestTerrainHeight(X, Y);
			if (z > WaterLevel + 1.0f)
			{
				if (T > 0.0f && M > 10.0f)
				{
					if (M > 70.0f)
						type = ID_Tree1;
					else if (M > 40.0f)
						type = ID_Tree2;
					else
						type = ID_Tree3;
				}
			}
		}
	}
	else if (x % 20 == 0 && y % 20 == 0)
	{
		if (trees > 0.0f && TemperatureNoise.GetPerlin_2D(X, Y, 2.0f) * 2.0f < trees)
		{
			z = GetTerrainHeight(X, Y, Z);
			if (z > WaterLevel + 1.0f)
			{
				if (T > 10.0f)
					type = ID_Grass2;
				else
					type = ID_Grass1;
			}
		}
	}

	if (type != ID_None)
	{
		outRotation.Yaw = FMath::RandHelper(360);
		float scale = FMath::RandHelper(100) * 0.01f;

		outScale.X = 0.5f + scale;
		outScale.Y = 0.5f + scale;
		outScale.Z = 0.5f + scale;

		outOffset.Z += z * VoxelSize;
	}

	return type;
}


void
FWorldGenOneInstance::GenerateFoilage(AInstancedFoliageActor& foliageActor)
{
	TArray<UInstancedStaticMeshComponent*> components;
	foliageActor.GetComponents<UInstancedStaticMeshComponent>(components);
	UInstancedStaticMeshComponent* meshComponent = nullptr;
	if (components.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No foilage components found (empty array)"));
	}
	else
	{
		meshComponent = components[0];
	}

	for (auto& c : components)
	{
		auto cs = c->InstanceStartCullDistance;
		auto ce = c->InstanceEndCullDistance;
		UE_LOG(LogTemp, Warning, TEXT("Component: %s, cull %d .. %d"), *c->GetName(), cs, ce);
	}

	int32 count = 0;

	if (!components.IsEmpty())
	{
		//Now you just need to add instances to component
		FTransform transform = FTransform();
		auto worldSize = WorldSize / 2;
		auto waterLevel = WaterLevel;
		bool skipAdding = false;
		FRotator rotation(0, 0, 0);
		FVector scale;
		FVector offset;
		for (int32 x = -worldSize / 2; x < worldSize / 2; x = x + 1)
		{
			for (int32 y = -worldSize / 2; y < worldSize / 2; y = y + 1)
			{
				int32 ft = -1;
				ft = GetFoilageType(x, y, 0, rotation, scale, offset);
				if (ft >= 0)
				{
					if (ft > components.Num() - 1)
						ft = 0;
					meshComponent = components[ft];
				}

				if (ft >= 0)
				{
					if (!skipAdding)
					{
						transform.SetLocation(FVector(VoxelSize * x + offset.X, VoxelSize * y + offset.Y, offset.Z));
						transform.SetRotation(rotation.Quaternion());
						transform.SetScale3D(scale);
						meshComponent->AddInstance(transform);
					}
					count++;
					if (count % 50000 == 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("Foliage instance count = %ld..."), count);
					}
					if (skipAdding == false && count > 200000)
					{
						UE_LOG(LogTemp, Warning, TEXT("TOO MANY foliage instances %ld - bailing out"), count);
						skipAdding = true;
					}
					// UE_LOG(LogTemp, Warning, TEXT("Spawn foilage at (%d, %d, %f) r=%f"), x, y, offset.Z, rotation.Yaw);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Spawned %ld foilages"), count);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No foilage components found (empty array)"));
	}
}