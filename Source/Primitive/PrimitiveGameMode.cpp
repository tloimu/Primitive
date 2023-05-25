// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveGameMode.h"
#include "PrimitiveCharacter.h"
#include "UObject/ConstructorHelpers.h"

APrimitiveGameMode::APrimitiveGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
