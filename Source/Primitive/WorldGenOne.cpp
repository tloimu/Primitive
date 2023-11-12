// Copyright 2021 Phyronnaz

#include "WorldGenOne.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "VoxelMaterialBuilder.h"
#include "OmaUtil.h"

float
Curve::GetValueAt(float inAt) const
{
	if (Points.empty())
		return inAt;

	if (inAt < -1.0f || inAt > 1.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Curve::GetValueAt(%f) exceeded valid input range -1.0..1.0"), inAt);
	}

	auto *prev = &Points[0];
	for (size_t i = 1; i < Points.size(); i++)
	{
		auto &cur = Points[i];
		if (inAt >= prev->first && inAt <= cur.first)
		{
			auto v = FMath::Lerp(prev->second, cur.second, ((inAt - prev->first) / (cur.first - prev->first)));
			return v;
		}
		prev = &Points[i];
	}
	return prev->second;
}

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

	Noise.SetSeed(Seed+1);
	WaterNoise.SetSeed(Seed + 10);
	TemperatureNoise.SetSeed(Seed + 100);
	MoistureNoise.SetSeed(Seed + 500);

	CurveMountains.Points.push_back(std::make_pair<float, float>(-1.0f, -1.0f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(-0.15f, -0.05f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(-0.10f, -0.02f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(-0.03f, -0.003f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(0.0f, 0.00f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(0.03f, 0.003f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(0.10f, 0.02f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(0.15f, 0.05f));
	CurveMountains.Points.push_back(std::make_pair<float, float>(1.0f, 1.0f));

	CurveIslands.Points.push_back(std::make_pair<float, float>(-1.0f, -1.0f));
	CurveIslands.Points.push_back(std::make_pair<float, float>(1.0f, 1.0f));

	CurveLowlands.Points.push_back(std::make_pair<float, float>(-1.0f, -1.0f));
	CurveLowlands.Points.push_back(std::make_pair<float, float>(1.0f, 1.0f));

	CurveSwamps.Points.push_back(std::make_pair<float, float>(-1.0f, -1.0f));
	CurveSwamps.Points.push_back(std::make_pair<float, float>(1.0f, 1.0f));
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
	int32 octaves = 11;

	return (TerrainHeight) * (
		+CurveMountains.GetValueAt(Noise.GetPerlinFractal_2D(X, Y, 0.00006f, octaves))
		);
}

float
FWorldGenOneInstance::GetMoisture(v_flt X, v_flt Y, v_flt Z) const
{
	float Klat = GetLatitude(Y);

	float Mnoise = (MoistureNoise.GetPerlin_2D(X, Y, MoistureVariationFreq)) * MoistureVariation;
	float Mraw = EquatorMoisture - (Klat * (EquatorMoisture - PolarMoisture)) + Mnoise - Z * 0.001;
	if (Mraw < 0.0f)
		Mraw = 0.0f;
	return Mraw;
}

float
FWorldGenOneInstance::GetTemperature(v_flt X, v_flt Y, v_flt Z) const
{
	float Klat = GetLatitude(Y);
	float Theight = Z > 0.0f ? TemperatureHeightCoeff * (Z * VoxelSize / 100.0f) : 0.0f;

	float Tnoise = (TemperatureNoise.GetPerlin_2D(X, Y, TemperatureVariationFreq))* TemperatureVariation;
	float T = EquatorTemperature - (Klat * (EquatorTemperature - PolarTemperature)) + Tnoise - Theight;

	return T;
}

float
FWorldGenOneInstance::GetLatitude(v_flt Y) const
{
	return 2 * (abs(Y) / WorldSize);
}

float
FWorldGenOneInstance::GetNearLowestTerrainHeight(v_flt X, v_flt Y) const
{
	float z = GetTerrainHeight(X, Y, 0);
	for (int dx = -1; dx < 2; dx = dx + 2)
	{
		for (int dy = -1; dy < 2; dy = dy + 2)
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

	if (Z <= 1.0l && Z > - 2.0l) // Water Level
	{
		// Sea ice
		if (GetTemperature(X, Y, Z) < -10.0f)
			return -1.0l;
	}

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// Value positive when Z > Height, and negative Z < Height
	float Value = Z - Height;

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5.0l;

	return Value;
}


FVoxelMaterial
FWorldGenOneInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);

	float Height = GetTerrainHeight(X, Y, Z);

	const int32 underWaterMaterial = 7;
	if (Z < -2.0l) // Water Level
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

float
FWorldGenOneInstance::TreeSize(float T, float M) const
{
	// Treeline is at about 6.5C (yearly average) +/-1.0C
	// Moisture positively correlates with tree size
	return (T - 6.5f) / 20.0f + (M - 60.0f) / 200.0f;
}

int32
FWorldGenOneInstance::GetFoilageType(v_flt X, v_flt Y, v_flt Z, FRotator& outRotation, FVector& outScale, FVector& outOffset) const
{
	int32 x = (int32)X;
	int32 y = (int32)Y;

	if (x % 10 != 0 || y % 10 != 0)
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

	float ox = FMath::FRandRange(-30.0f, 30.0f);
	float oy = FMath::FRandRange(-30.0f, 30.0f);
	outOffset.X = ox * VoxelSize;
	outOffset.Y = oy * VoxelSize;
	X += ox;
	Y += oy;
	float scale = FMath::FRandRange(0.5f, 1.5f);

	auto rocks = (WaterNoise.GetPerlin_2D(X, Y, 0.001f) + WaterNoise.GetPerlin_2D(X, Y, 0.02f));
	auto trees = (TemperatureNoise.GetPerlin_2D(X, Y, 0.001f) + TemperatureNoise.GetPerlin_2D(X, Y, 0.02f));

	if (rocks > 0.5f && (x % 40 == 0 && y % 40 == 0))
	{
		if (T > -15.0f && WaterNoise.GetPerlin_2D(X, Y, 10.0f) * 3.5f < rocks)
		{
			z = GetNearLowestTerrainHeight(X, Y);
			if (FMath::RandRange(0.0f, 1.0f) > 0.5f)
				type = ID_Rock1;
			else
				type = ID_Rock2;
			outOffset.Z = FMath::FRandRange(-200.0f, 0.0f);
			outRotation.Pitch = FMath::FRandRange(0.0f, 360.0f);
			outRotation.Roll = FMath::FRandRange(0.0f, 360.0f);
		}
	}
	else if (trees > 0.0f && (x % 60 == 0 && y % 60 == 0))
	{
		float s = TreeSize(T, M);
		if (s > 0.3f)
		{
			if (TemperatureNoise.GetPerlin_2D(X, Y, 10.0f) * 2.0f < trees)
			{
				z = GetNearLowestTerrainHeight(X, Y);
				if (z > 1.0f) // Above Water Level
				{
					if (T > 6.5f && M > 10.0f)
					{
						if (T < 20.0f)
							type = ID_Tree1_Mod;
						else if (M > 70.0f)
							type = ID_Tree1;
						else if (M > 55.0f)
							type = ID_Tree2;
						else
							type = ID_Tree3;

						scale = scale * s;
					}
				}
			}
		}
	}
	else if (x % 10 == 0 && y % 10 == 0)
	{
		if (trees > 0.0f && trees < 0.3f)
		{
			z = GetTerrainHeight(X, Y, Z);
			if (z > 1.0f) // Above Water Level
			{
				if (T > 10.0f)
					type = ID_Grass2;
				else
					type = ID_Grass1;
				if (T > 8.0f)
				{
					float plants = FMath::RandRange(0.0f, 1.0f);
					if (plants > 0.95f)
						type = ID_Plant1;
					else if (plants > 0.9f)
						type = ID_Plant2;
				}
			}
			else
			{
				// TODO: Underwater flora
			}
		}
	}

	if (type != ID_None)
	{
		outRotation.Yaw = FMath::FRandRange(0.0f, 360.0f);
		outScale = outScale.One() * scale;

		outOffset.Z += z * VoxelSize;
	}

	return type;
}

void
FWorldGenOneInstance::GenerateFoliage(TArray<UInstancedStaticMeshComponent*>& components)
{
	OmaUtil::MsTimer timer;

	UInstancedStaticMeshComponent* meshComponent = nullptr;
	if (components.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No foilage components found (empty array)"));
	}
	else
	{
		meshComponent = components[0];
	}

	int ci = 0;
	for (auto& c : components)
	{
		auto cs = c->InstanceStartCullDistance;
		auto ce = c->InstanceEndCullDistance;
		UE_LOG(LogTemp, Warning, TEXT("Component %d: %s, cull %d .. %d"), ci, *c->GetName(), cs, ce);
		// ???? TODO: Make foliage component ID discovery smarter and more configurable
		auto name = c->GetName();
		if (name.Contains("Rock1"))
			ID_Rock1 = ci;
		else if (name.Contains("Rock2"))
			ID_Rock2 = ci;
		else if (name.Contains("Tree1"))
			ID_Tree1 = ci;
		else if (name.Contains("Tree2"))
			ID_Tree2 = ci;
		else if (name.Contains("Tree3"))
			ID_Tree3 = ci;
		else if (name.Contains("Tree1_Mod"))
			ID_Tree1_Mod = ci;
		else if (name.Contains("Grass1"))
			ID_Grass1 = ci;
		else if (name.Contains("Grass2"))
			ID_Grass2 = ci;
		else if (name.Contains("Plant1"))
			ID_Plant1 = ci;
		else if (name.Contains("Plant2"))
			ID_Plant2 = ci;
		ci++;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("Foilage setup rock1=%d, tree1=%d, tree2=%d, tree3=%d, grass1=%d, grass2=%d"),
		ID_Rock1, ID_Tree1, ID_Tree2, ID_Tree3, ID_Grass1, ID_Grass2);

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
		FRotator rotation;
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

	ci = 0;
	for (auto& c : components)
	{
		c->AddInstances(NewInstancesPerComponent[ci], false);
		UE_LOG(LogTemp, Warning, TEXT("Component: %s, instances %d"), *c->GetName(), NewInstancesPerComponent[ci].Num());
		ci++;
	}
	timer.LogAndCheck("Foilage adding time");
	timer.LogTotal("Foilage time");
}