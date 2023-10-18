// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitivePlayerController.h"

void
APrimitivePlayerController::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("APrimitivePlayerController: BeginPlay"));
}

void
APrimitivePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("APrimitivePlayerController: EndPlay"));
}
