// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrimitiveGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UPrimitiveGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	void Init() override;
	void Shutdown() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UItemDatabase* ItemDb = nullptr;

	void SetupGameConfig();
};
