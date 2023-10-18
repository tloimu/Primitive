// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PrimitivePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API APrimitivePlayerController : public APlayerController
{
	GENERATED_BODY()
	
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
