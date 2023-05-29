// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	Health = 100.0f;
}

void UHUDWidget::SetHealth(float Value)
{
	Health = Value;
}
