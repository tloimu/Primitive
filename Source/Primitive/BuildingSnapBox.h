// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BuildingSnapBox.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Collision", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "Snap Box Collision", BlueprintSpawnableComponent))
class PRIMITIVE_API UBuildingSnapBox : public UBoxComponent
{
	GENERATED_BODY()

public:

	UBuildingSnapBox(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|SnapBox") FRotator PlaceRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|SnapBox") FVector PlaceOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|SnapBox") bool PlaceStacksUp = false;
};

extern FName GAME_TAG_BUILDING;
