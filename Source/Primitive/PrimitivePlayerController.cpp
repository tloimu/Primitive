// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitivePlayerController.h"

void
APrimitivePlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("APrimitivePlayerController: BeginPlay"));
}

void
APrimitivePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Warning, TEXT("APrimitivePlayerController: EndPlay"));
}
