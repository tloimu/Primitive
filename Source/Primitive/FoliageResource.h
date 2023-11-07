// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
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
class PRIMITIVE_API UHISMFoliage : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FResourceItems>	BreaksIntoItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HitPoints;
};
