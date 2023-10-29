// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameState.h"
#include "PrimitiveGameInstance.h"
#include "Kismet/GameplayStatics.h"

void
APrimitiveGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameState: BeginPlay"));

	if (!SunLight)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			SunLight = FoundActors[0];
			UE_LOG(LogTemp, Warning, TEXT("Surrogate SunLight actor found as the first ADirectionalLight in the level"));
		}
	}

	if (SunLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("SunLight actor found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SunLight actor not found"));
	}

	LastCheckOfDayTimeSince = CheckDayTimeIntervalGameSeconds + 1.0f;

	auto gi = Cast<UPrimitiveGameInstance>(GetGameInstance());
	if (gi)
	{
		gi->ReadGameSave();
	}
}

void
APrimitiveGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckSunlight(DeltaSeconds);
	CheckWeather(DeltaSeconds);
}

void
APrimitiveGameState::CheckSunlight(float DeltaSeconds)
{
	LastCheckOfDayTimeSince += DeltaSeconds;
	if (LastCheckOfDayTimeSince > CheckDayTimeIntervalGameSeconds)
	{
		UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameState::CheckSunLight"));

		if (SunLight)
		{
			float clockAdvance = LastCheckOfDayTimeSince * ClockSpeed;
			if (FMath::TruncToInt(ClockInSecs / (60 * 60.0f)) != FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)))
			{
				UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameState::Hour = %d"), FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)));
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
		LastCheckOfDayTimeSince = 0.0f;
	}
}

void
APrimitiveGameState::CheckWeather(float DeltaSeconds)
{
}
