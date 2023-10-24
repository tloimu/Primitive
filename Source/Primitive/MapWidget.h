// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup();
	void GenerateMap();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) class UImage* MapImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) class UImage* HeightMapImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) class UImage* TemperatureMapImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) class UImage* MoistureMapImage;
};
