// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveGameMode.h"
#include "PrimitiveCharacter.h"
#include "CoreFwd.h"
#include "Framework/Application/NavigationConfig.h"
#include "UObject/ConstructorHelpers.h"

APrimitiveGameMode::APrimitiveGameMode() : AGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Prevent Widgets (like Inventory) to consume Tab-key events
	FNavigationConfig& NavigationConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig.bTabNavigation = false;
}

void
APrimitiveGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Mode: BeginPlay"));
}

void
APrimitiveGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Mode: EndPlay"));
}
