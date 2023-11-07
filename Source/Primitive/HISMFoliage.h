// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "FoliageResource.h"
#include "HISMFoliage.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UHISMFoliage : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FResourceItems>	BreaksIntoItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HitPoints;
};
