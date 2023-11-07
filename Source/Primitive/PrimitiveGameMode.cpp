// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveGameMode.h"
#include "PrimitiveCharacter.h"
#include "PrimitiveGameInstance.h"
#include "CoreFwd.h"
#include "UObject/ConstructorHelpers.h"
#include "OmaUtil.h"

APrimitiveGameMode::APrimitiveGameMode() : AGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	OmaUtil::UseTabNavigationInWidgets(false);
}

void
APrimitiveGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Game Mode: BeginPlay"));
}

void
APrimitiveGameMode::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogTemp, Warning, TEXT("Game Mode: StartPlay"));
	auto gi = Cast<UPrimitiveGameInstance>(GetGameInstance());
	if (gi)
	{
		gi->SetupGame();
	}
}

void
APrimitiveGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Warning, TEXT("Game Mode: EndPlay"));
}
