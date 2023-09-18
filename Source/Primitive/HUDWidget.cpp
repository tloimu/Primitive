// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	Health = 90.0f;
	Stamina = 100.0f;
	Temperature = 0.0f;
	Moisture = 0.0f;
	ClockInHoursOfDay = 0.0f;
}

void
UHUDWidget::SetHealth(float Value)
{
	Health = Value;
}

void
UHUDWidget::SetStamina(float Value)
{
	Stamina = Value;
}

void
UHUDWidget::SetEnvironment(float T, float M)
{
	Temperature = T;
	Moisture = M;
}

void
UHUDWidget::SetLocation(const FIntVector& Loc, float Lat)
{
	Location = Loc;
	Latitude = Lat;
}

void
UHUDWidget::SetTerrainHeight(int H)
{
	TerrainHeight = H;
}

void
UHUDWidget::SetClock(float HoursOfDay)
{
	ClockInHoursOfDay = HoursOfDay;
}