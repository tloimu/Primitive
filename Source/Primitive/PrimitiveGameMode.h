// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldGenOne.h"
#include "PrimitiveGameMode.generated.h"

UCLASS(minimalapi)
class APrimitiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrimitiveGameMode();

	void BeginPlay() override;

	void GenerateFoilage();

	FWorldGenOneInstance* WorldGenInstance;
};
