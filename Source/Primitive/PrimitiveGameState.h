// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Engine/SkyLight.h"
#include "Sound/SoundCue.h"
#include "PrimitiveGameState.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API APrimitiveGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	APrimitiveGameState();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	// Environmental variables and functionality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	float ClockInSecs = 17*60*60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	int32 Day = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	int32 DayOfYear = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")//, Replicated)
	float ClockSpeed = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<class ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<class ASkyLight> SkyLight;

	float LastCheckOfDayTimeSince = 0.0f;
	float CheckDayTimeIntervalGameSeconds = 0.25f;

	void CheckSunlight(float DeltaSeconds);
	void CheckWeather(float DeltaSeconds);

	UPROPERTY(EditAnywhere) USoundCue* MorningSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* MiddaySound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* NightfallSound = nullptr;

	float NightfallSeconds, MorningSeconds, MiddaySeconds;

	void OnMorning();
	void OnMidday();
	void OnNightfall();

	void PlaySound(USoundCue* inDefaultSound, USoundCue* inOverrideSound = nullptr) const;

	UFUNCTION(Exec, Category = ExecFunctions) void prisethour(int hour);
};
