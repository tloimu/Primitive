// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameState.h"
#include "PrimitiveGameInstance.h"
#include "Kismet/GameplayStatics.h"

APrimitiveGameState::APrimitiveGameState()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	SetActorTickInterval(0.3f);

	NightfallSeconds = 18 * 3600 + 20 * 60;
	MorningSeconds = 5 * 3600 + 40 * 60;
	MiddaySeconds = 12 * 3600;
}

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
		if (SunLight)
		{
			float clockAdvance = LastCheckOfDayTimeSince * ClockSpeed;
			if (FMath::TruncToInt(ClockInSecs / (60 * 60.0f)) != FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)))
			{
				UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameState::Hour = %d"), FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)));
			}

			if (ClockInSecs < NightfallSeconds && ClockInSecs + clockAdvance >= NightfallSeconds)
			{
				OnNightfall();
			}

			if (ClockInSecs < MorningSeconds && ClockInSecs + clockAdvance >= MorningSeconds)
			{
				OnMorning();
			}

			if (ClockInSecs < MiddaySeconds && ClockInSecs + clockAdvance >= MiddaySeconds)
			{
				OnMidday();
			}

			ClockInSecs += clockAdvance;
			if (ClockInSecs > 24 * 60 * 60.0f)
			{
				Day++;
				DayOfYear++;
				ClockInSecs -= 24 * 60 * 60.0f;
				UE_LOG(LogTemp, Warning, TEXT("Start of Day %d"), Day);
				if (DayOfYear > 365)
				{
					DayOfYear = 1;
					UE_LOG(LogTemp, Warning, TEXT("New Year"));
				}
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
APrimitiveGameState::PlaySound(USoundCue* inDefaultSound, USoundCue* inOverrideSound) const
{
	auto sound = inOverrideSound ? inOverrideSound : inDefaultSound;
	if (sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), sound);
	}
}


void
APrimitiveGameState::OnMorning()
{
	UE_LOG(LogTemp, Warning, TEXT("Morning"));

	PlaySound(MorningSound);

}

void
APrimitiveGameState::OnMidday()
{
	UE_LOG(LogTemp, Warning, TEXT("Midday"));

	PlaySound(MiddaySound);
}

void
APrimitiveGameState::OnNightfall()
{
	UE_LOG(LogTemp, Warning, TEXT("Nightfall"));

	PlaySound(NightfallSound);
}


void
APrimitiveGameState::CheckWeather(float DeltaSeconds)
{
}

void
APrimitiveGameState::prisethour(int hour)
{
	ClockInSecs = hour * 60 * 60.0f;
}
