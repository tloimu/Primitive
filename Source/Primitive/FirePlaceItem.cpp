// Fill out your copyright notice in the Description page of Project Settings.


#include "FirePlaceItem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AFirePlaceItem::AFirePlaceItem(const FObjectInitializer& Initializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1.0f);
	SetActorTickEnabled(true);
}

bool
AFirePlaceItem::Interact_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact fire place"));

	if (CurrentState)
		CurrentState = 0;
	else
		CurrentState = 1;

	CheckFireEffect();

	return true;
}

void
AFirePlaceItem::CheckFireEffect()
{
	if (!FireEffect && FireEffectSystem)
	{
		FireEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffectSystem, GetActorLocation(), FRotator::ZeroRotator, FVector(1.0f), true, false);
	}

	if (FireEffect)
	{
		if (CurrentState)
		{
			if (!FireEffect->IsActive())
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire place on"));
				FireEffect->Activate();
				SetActorTickEnabled(true);
			}
		}
		else
		{
			if (FireEffect->IsActive())
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire place off"));
				FireEffect->Deactivate();
				FireEffect = nullptr;
				SetActorTickEnabled(false);
			}
		}
	}
}

void
AFirePlaceItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentState)
	{
		float gameWorldTimeDelta = DeltaSeconds * GetClockSpeed();
		HeatItems(gameWorldTimeDelta);
		CheckFuel(gameWorldTimeDelta);
	}
}

void
AFirePlaceItem::OnLoaded()
{
	CheckFireEffect();
}

void
AFirePlaceItem::HeatItems(float DeltaSeconds)
{
	if (Inventory)
	{
		for (auto& slot : Inventory->Slots)
		{
			if (slot.Count > 0 && slot.Item.UsableFor.Contains(EItemUtility::HeatSolid))
			{
				// TODO
			}
		}
	}
}

void
AFirePlaceItem::CheckFuel(float DeltaSeconds)
{
	if (FuelLeftSeconds <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire place checking for more fuel"));
		for (auto& slot : Inventory->Slots)
		{
			if (slot.Count > 0 && slot.Item.UsableFor.Contains(EItemUtility::SolidFuel))
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire place consuming %s as fuel"), *slot.Item.Id);
				slot.Count = slot.Count - 1;
				FuelLeftSeconds = 20 * 60.0f; // TODO: Get this from the fuel item
				return;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Fire place out of fuel"));
		CurrentState = 0;
		CheckFireEffect();
	}
	else
	{
		FuelLeftSeconds -= DeltaSeconds;
	}
}
