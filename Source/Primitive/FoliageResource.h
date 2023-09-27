// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ItemStruct.h"
#include "FoliageResource.generated.h"



USTRUCT(BlueprintType)
struct FResourceItems
{
    GENERATED_BODY()

    FResourceItems() : Count(1), Quality(1.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Quality;
};


UCLASS()
class PRIMITIVE_API UFoliageResource : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:

	UFoliageResource() : UInstancedStaticMeshComponent(), HitPoints(0) {}
	UFoliageResource(const FObjectInitializer& ObjectInitializer) : UInstancedStaticMeshComponent(ObjectInitializer), HitPoints(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FResourceItems>	BreaksIntoItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HitPoints;
};
