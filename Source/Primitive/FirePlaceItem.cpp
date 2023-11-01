// Fill out your copyright notice in the Description page of Project Settings.


#include "FirePlaceItem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AFirePlaceItem::AFirePlaceItem(const FObjectInitializer& Initializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1.0f);
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
		FireEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffectSystem, GetActorLocation());
	}

	if (FireEffect)
	{
		if (CurrentState)
		{
			if (!FireEffect->IsActive())
			{
				FireEffect->Activate();
				SetActorTickEnabled(true);
			}
		}
		else
		{
			if (FireEffect->IsActive())
			{
				FireEffect->Deactivate();
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
	FuelLeftSeconds -= DeltaSeconds;
	if (FuelLeftSeconds <= 0.0f)
	{
		for (auto& slot : Inventory->Slots)
		{
			if (slot.Count > 0 && slot.Item.UsableFor.Contains(EItemUtility::SolidFuel))
			{
				slot.Count = slot.Count - 1;
				FuelLeftSeconds = 20 * 60.0f; // TODO: Get this from the fuel item
				return;
			}
		}
	}

	CurrentState = 0;
	CheckFireEffect();
}
