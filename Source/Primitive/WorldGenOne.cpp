// Copyright 2021 Phyronnaz

#include "WorldGenOne.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "VoxelMaterialBuilder.h"

TVoxelSharedRef<FVoxelGeneratorInstance> UWorldGenOne::GetInstance()
{
	auto p = MakeVoxelShared<FWorldGenOneInstance>(*this);
	FWorldGenOneInstance::sGeneratorInstance = p;
	return p;
}

TSharedPtr<FWorldGenOneInstance> FWorldGenOneInstance::sGeneratorInstance; // ???? DIRTY!

///////////////////////////////////////////////////////////////////////////////
/*
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
	, MaxFoliageInstances(MyGenerator.MaxFoliageInstances)
	, MaxFoliageRange(MyGenerator.MaxFoliageRange)
	, TerrainHeight(MyGenerator.TerrainHeight)
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
	WorldSize = InitStruct.WorldSize;
	VoxelSize = InitStruct.VoxelSize;

	Noise.SetSeed(Seed);
	WaterNoise.SetSeed(Seed + 10);
	TemperatureNoise.SetSeed(Seed + 100);
	MoistureNoise.SetSeed(Seed + 500);
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
	// float ErosionFromMoisture = FMath::Clamp(1.0f - GetMoisture(X, Y, Z) / 110.0f, 0.4f, 1.0f);
	float flatness = FMath::Clamp(Noise.GetPerlin_2D(X, Y, 0.0001f), 0.3f, 2.0f);

	int32 octaves = 11 + 2 * Noise.GetPerlin_2D(X, Y, 0.001f);

	return (TerrainHeight * flatness) * (
		+ Noise.GetSimplexFractal_2D(X, Y, 0.00006f, octaves)
		);
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

	float Tnoise = (TemperatureNoise.GetPerlin_2D(X, Y, TemperatureVariationFreq))* TemperatureVariation;
	float T = EquatorTemperature - (Klat * (EquatorTemperature - PolarTemperature)) + Tnoise - TemperatureHeightCoeff * (Z * VoxelSize / 100);

	return T;
}

float
FWorldGenOneInstance::GetLatitude(v_flt Y) const
{
	return 2 * (abs(Y) / WorldSize);
}

const int32 ID_None = -1;
const int32 ID_Grass1 = 0;
const int32 ID_Grass2 = 1;
const int32 ID_Rock1 = 2;
const int32 ID_Tree1 = 3; // Mid-sized tree
const int32 ID_Tree2 = 4; // Big tree
const int32 ID_Tree3 = 5; // Small tree

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

	if (Z == 0) // Water Level
	{
		// Sea ice
		if (GetTemperature(X, Y, Z) < -10.0f)
			return -1;
	}

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

	const int32 underWaterMaterial = 7;
	if (Z < -1.0f) // Water Level
	{
		Builder.AddMultiIndex(underWaterMaterial, 1.0f);
	}
	else
	{
		float T = GetTemperature(X, Y, Z);
		float M = GetMoisture(X, Y, Z);

		const int32 snowMaterial = 9;
		const int32 coldMaterial = 8;
		const int32 temperateMaterial = 1;
		const int32 hotMaterial = M < 30.0f ? 7 : 2;

		// ???? TODO: Add moisture's effects to snow thickness and vegetation

		float SnowThickness = T < -5.0f ? -(T + 5.0f) * 1.0f : 0.0f;

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
	}

	return Builder.Build();
}


int32
FWorldGenOneInstance::GetFoilageType(v_flt X, v_flt Y, v_flt Z, FRotator& outRotation, FVector& outScale, FVector& outOffset) const
{
	int32 x = (int32)X;
	int32 y = (int32)Y;

	if (x % 20 != 0 || y % 20 != 0)
		return ID_None;

	auto T = GetTemperature(X, Y, Z);
	auto M = GetMoisture(X, Y, Z);
	if (T < -2.0f || M < 5.0f) // Too cold an/or dry
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

	if (rocks > 0.5f && (x % 40 == 0 && y % 40 == 0))
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
	else if (trees > 0.0f && (x % 60 == 0 && y % 60 == 0))
	{
		if (TemperatureNoise.GetPerlin_2D(X, Y, 10.0f) * 2.0f < trees)
		{
			z = GetNearLowestTerrainHeight(X, Y);
			if (z > 1.0f) // Above Water Level
			{
				if (T > 0.0f && M > 10.0f)
				{
					if (M > 70.0f)
						type = ID_Tree2;
					else if (M > 55.0f)
						type = ID_Tree1;
					else
						type = ID_Tree3;
				}
			}
		}
	}
	else if (x % 40 == 0 && y % 40 == 0)
	{
		if (trees > 0.0f && TemperatureNoise.GetPerlin_2D(X, Y, 2.0f) * 2.0f < trees)
		{
			z = GetTerrainHeight(X, Y, Z);
			if (z > 1.0f) // Above Water Level
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


class MsTimer
{
public:
	int64 started = 0;
	int64 checked = 0;

	MsTimer() { Start(); }

	void Start()
	{
		started = FDateTime::UtcNow().GetTicks();
		checked = FDateTime::UtcNow().GetTicks();
	}

	int32 Check()
	{
		auto check = SinceCheck();
		checked = FDateTime::UtcNow().GetTicks();
		return check;
	}

	int32 SinceCheck()
	{
		auto now = FDateTime::UtcNow().GetTicks();
		return (now - checked) / 10000;
	}

	int32 Total()
	{
		auto now = FDateTime::UtcNow().GetTicks();
		return (now - started) / 10000;
	}
};

void
FWorldGenOneInstance::GenerateFoilage(AInstancedFoliageActor& foliageActor)
{
	MsTimer timer;

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

	auto setupMs = FDateTime::UtcNow().GetTicks();
	UE_LOG(LogTemp, Warning, TEXT("Foilage setup time: %d, %d max instances"), timer.Check(), MaxFoliageInstances);

	int32 count = 0;

	TArray<TArray<FTransform> > NewInstancesPerComponent;
	for (auto& c : components)
	{
		NewInstancesPerComponent.AddDefaulted();
		NewInstancesPerComponent.Reserve(MaxFoliageInstances);
	}

	if (!components.IsEmpty())
	{
		//Now you just need to add instances to component
		// foliageActor.bAutoRebuildTreeOnInstanceChanges;
		FTransform transform = FTransform();
		auto worldSize = WorldSize;
		if (MaxFoliageRange > 0)
			worldSize = MaxFoliageRange;
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
						NewInstancesPerComponent[ft].Add(transform);
					}
					count++;
					if (count % 50000 == 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("Foliage instance count = %ld..."), count);
					}
					if (skipAdding == false && count > MaxFoliageInstances)
					{
						UE_LOG(LogTemp, Warning, TEXT("TOO MANY foliage instances %ld - bailing out"), count);
						skipAdding = true;
						auto generateUntilBailMs = FDateTime::UtcNow().GetTicks();
						UE_LOG(LogTemp, Warning, TEXT("Foilage generation time until bailing out: %ld"), timer.SinceCheck());
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

	UE_LOG(LogTemp, Warning, TEXT("Foilage generation time: %d"), timer.Check());

	int32 ci = 0;
	for (auto& c : components)
	{
		c->AddInstances(NewInstancesPerComponent[ci], false);
		UE_LOG(LogTemp, Warning, TEXT("Component: %s, instances %d"), *c->GetName(), NewInstancesPerComponent[ci].Num());
		ci++;
	}
	UE_LOG(LogTemp, Warning, TEXT("Foilage adding time: %d"), timer.Check());

	auto totalMs = FDateTime::UtcNow().GetTicks();
	UE_LOG(LogTemp, Warning, TEXT("Foilage total time: %d"), timer.Total());
}