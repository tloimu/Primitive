// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrimitiveGameMode.generated.h"

UCLASS(minimalapi)
class APrimitiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrimitiveGameMode();

	void BeginPlay() override;
	void StartPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
