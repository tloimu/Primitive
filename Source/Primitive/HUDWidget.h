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

	UFUNCTION(BlueprintCallable) void SetHealth(float Value);
	UFUNCTION(BlueprintCallable) void SetStamina(float Value);
	UFUNCTION(BlueprintCallable) void SetEnvironment(float T, float M);
	UFUNCTION(BlueprintCallable) void SetLocation(const FIntVector &Loc, float Lat);
	UFUNCTION(BlueprintCallable) void SetClock(float HoursOfDay);

	UFUNCTION(BlueprintCallable) float GetTemperature() const { return Temperature; }
	UFUNCTION(BlueprintCallable) float GetMoisture() const { return Moisture; }
	UFUNCTION(BlueprintCallable) FIntVector GetLocation() const { return Location; }

protected:

	UPROPERTY(EditAnywhere, Transient) float Health;
	UPROPERTY(EditAnywhere, Transient) float Stamina;
	UPROPERTY(EditAnywhere, Transient) float Temperature;
	UPROPERTY(EditAnywhere, Transient) float Moisture;
	UPROPERTY(EditAnywhere, Transient) float Latitude;
	UPROPERTY(EditAnywhere, Transient) float ClockInHoursOfDay;
	UPROPERTY(EditAnywhere, Transient) FIntVector Location;
};
