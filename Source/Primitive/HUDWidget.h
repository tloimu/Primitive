// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:

	UFUNCTION(BlueprintCallable) void SetHealth(float Value);

	UPROPERTY(EditAnywhere) float Health;
};
