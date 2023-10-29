// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Engine/SkyLight.h"
#include "PrimitiveGameState.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API APrimitiveGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	// Environmental variables and functionality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	float ClockInSecs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	int32 Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	int32 DayOfYear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	float ClockSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<class ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<class ASkyLight> SkyLight;

	float LastCheckOfDayTimeSince = 0.0f;
	float CheckDayTimeIntervalGameSeconds = 0.5f;

	void CheckSunlight(float DeltaSeconds);
	void CheckWeather(float DeltaSeconds);
};
