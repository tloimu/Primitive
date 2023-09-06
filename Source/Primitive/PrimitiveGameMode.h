// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldGenOne.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Engine/SkyLight.h"
#include "PrimitiveGameMode.generated.h"

UCLASS(minimalapi)
class APrimitiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrimitiveGameMode();

	void BeginPlay() override;

	void GenerateFoilage();

	void Tick(float DeltaTime) override;

	FWorldGenOneInstance* WorldGenInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool DoGenerateFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float ClockInSecs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	int32 Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	int32 DayOfYear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float ClockSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<ASkyLight> SkyLight;
};
