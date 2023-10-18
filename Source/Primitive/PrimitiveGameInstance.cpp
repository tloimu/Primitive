// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameInstance.h"
#include "ItemDatabase.h"

void
UPrimitiveGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Instance: Init"));

	SetupGameConfig();
}

void
UPrimitiveGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Instance: Shutdown"));
}

void
UPrimitiveGameInstance::SetupGameConfig()
{
	if (ItemDb)
	{
		ItemDb->SetupItems();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Item Database found"));
	}
}