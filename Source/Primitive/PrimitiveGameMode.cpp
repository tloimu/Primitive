// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveGameMode.h"
#include "PrimitiveCharacter.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "CoreFwd.h"
#include "UObject/ConstructorHelpers.h"

APrimitiveGameMode::APrimitiveGameMode() : AGameModeBase(), WorldGenInstance(nullptr), DoGenerateFoliage(true), ClockInSecs(12 * 60 * 60.0f), Day(1), DayOfYear(6 * 30), ClockSpeed(600.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void
APrimitiveGameMode::BeginPlay()
{
	if (DoGenerateFoliage)
		GenerateFoilage();

	if (SunLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("SunLight actor found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SunLight actor NOT found"));
	}
}

void
APrimitiveGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameMode::Tick %f"), DeltaTime);
	if (SunLight)
	{
		float clockAdvance = DeltaTime * ClockSpeed;
		if (FMath::TruncToInt(ClockInSecs / (60 * 60.0f)) != FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)))
		{
			UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameMode::Hour = %d"), FMath::TruncToInt((ClockInSecs + clockAdvance) / (60*60.0f)));
		}
		ClockInSecs += clockAdvance;
		if (ClockInSecs > 24 * 60 * 60.0f)
		{
			Day++;
			DayOfYear++;
			ClockInSecs -= 24 * 60 * 60.0f;
			UE_LOG(LogTemp, Warning, TEXT("Start of Day %d"), Day);
		}

		FRotator rot;
		rot.Pitch = ClockInSecs * 360.0f / (24 * 60 * 60.0f) + 90.0f;
		SunLight->SetActorRotation(rot);
		if (SkyLight)
		{
		}
	}
}


void
APrimitiveGameMode::GenerateFoilage()
{
	WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	TActorIterator<AInstancedFoliageActor> foliageIterator(GetWorld());
	if (foliageIterator)
	{
		AInstancedFoliageActor* foliageActor = *foliageIterator;

		if (WorldGenInstance)
		{
			WorldGenInstance->GenerateFoilage(*foliageActor);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No World Generator Instance found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No foilage actor found"));
	}
}

