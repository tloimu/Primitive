// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveGameMode.h"
#include "PrimitiveCharacter.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "CoreFwd.h"
#include "UObject/ConstructorHelpers.h"

APrimitiveGameMode::APrimitiveGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		WorldGenInstance = nullptr;
	}
}


void
APrimitiveGameMode::BeginPlay()
{
	WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	GenerateFoilage();
}


void
APrimitiveGameMode::GenerateFoilage()
{
	/*
	UInstancedStaticMeshComponent* meshComponent = NewObject<UInstancedStaticMeshComponent>(foliageActor, UInstancedStaticMeshComponent::StaticClass(), NAME_None, RF_Transactional);
	meshComponent->AttachTo(foliageActor->GetRootComponent());
	meshComponent->SetStaticMesh(MyStaticMesh);
	meshComponent->RegisterComponent();
	*/

	TActorIterator<AInstancedFoliageActor> foliageIterator(GetWorld());
	if (foliageIterator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Foilage actor found"));
		AInstancedFoliageActor* foliageActor = *foliageIterator;

		//if you already have foliage in your level, you just need to get the right component, one is created for each type
		TArray<UInstancedStaticMeshComponent*> components;
		foliageActor->GetComponents<UInstancedStaticMeshComponent>(components);
		UInstancedStaticMeshComponent* meshComponent = nullptr;
		if (components.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("No foilage components found (empty array)"));
/*			meshComponent = NewObject<UInstancedStaticMeshComponent>(foliageActor, UInstancedStaticMeshComponent::StaticClass(), NAME_None, RF_Transactional);
			//meshComponent->AttachTo(foliageActor->GetRootComponent());
			//meshComponent->SetStaticMesh(MyStaticMesh);
			meshComponent->RegisterComponent();
			foliageActor->GetComponents<UInstancedStaticMeshComponent>(components);*/
		}
		else
		{
			meshComponent = components[0];
		}

		for (auto& c : components)
		{
			UE_LOG(LogTemp, Warning, TEXT("Component: %s"), *c->GetName());
		}

		int32 count = 0;

		if (!components.IsEmpty())
		{
			//Now you just need to add instances to component
			FTransform transform = FTransform();
			auto worldSize = WorldGenInstance ? WorldGenInstance->GetWorldSize() /2 : 3000;
			auto waterLevel = WorldGenInstance ? WorldGenInstance->GetWaterLevel() : 0.0f;
			FRotator rotation(0, 0, 0);
			FVector scale;
			FVector offset;
			for (int32 x = -worldSize / 2; x < worldSize/2; x = x + 1)
			{
				for (int32 y = -worldSize/2; y < worldSize/2; y = y + 1)
				{
					int32 ft = -1;
					if (WorldGenInstance)
					{
						ft = WorldGenInstance->GetFoilageType(x, y, 0, rotation, scale, offset);
						if (ft >= 0)
						{
							if (ft > components.Num() - 1)
								ft = 0;
							meshComponent = components[ft];
						}
					}
					if (ft >= 0)
					{
						transform.SetLocation(FVector(20.0f * x + offset.X, 20.f * y + offset.Y, offset.Z));
						transform.SetRotation(rotation.Quaternion());
						transform.SetScale3D(scale);
						meshComponent->AddInstance(transform);
						count++;
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No foilage actor found"));
	}
}