// Fill out your copyright notice in the Description page of Project Settings.


#include "HISMFoliageActor.h"
#include "HISMFoliage.h"

// Sets default values
AHISMFoliageActor::AHISMFoliageActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	/*
	FoliageComponent = CreateDefaultSubobject<UHISMFoliage>(TEXT("HISMFoliage"));
	SetRootComponent(FoliageComponent);
	FoliageComponent->SetMobility(EComponentMobility::Static);
	FoliageComponent->SetCollisionProfileName("BlockAll");

	*/
}

// Called when the game starts or when spawned
void AHISMFoliageActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("HISM Actor BeginPlay"));
	TArray<UHierarchicalInstancedStaticMeshComponent*> components;
	GetComponents<UHierarchicalInstancedStaticMeshComponent>(components);
	for (auto c : components)
	{
		auto cs = c->InstanceStartCullDistance;
		auto ce = c->InstanceEndCullDistance;
		UE_LOG(LogTemp, Warning, TEXT("HISM Component: %s, cull %d .. %d, instances %ld"), *c->GetName(), cs, ce, c->GetInstanceCount());
	}
}

