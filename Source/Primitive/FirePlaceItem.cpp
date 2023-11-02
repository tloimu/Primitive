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

void
AFirePlaceItem::BeginPlay()
{
	Super::BeginPlay();

	for (auto c : GetComponents())
	{
		if (c->GetName() == FireWoodMeshName)
		{
			FireWoodMesh = Cast<UStaticMeshComponent>(c);
			break;
		}
	}

	if (Inventory)
	{
		Inventory->AddInventoryListener(*this);
	}
}

void
AFirePlaceItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Inventory)
	{
		Inventory->RemoveInventoryListener(*this);
	}
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
	if (Inventory)
	{
		Inventory->AddInventoryListener(*this);
	}

	CheckFireEffect();
	CheckHasFuelStatus();
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
		FItemStruct item;
		if (Inventory->ConsumeItemOfUtility(EItemUtility::SolidFuel, item))
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire place consuming %s as fuel"), *item.Id);
			FuelLeftSeconds = 20 * 60.0f; // TODO: Get this from the fuel <item>
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Fire place out of fuel"));
		CurrentState = 0;
		CheckFireEffect();
		if (FireWoodMesh)
			FireWoodMesh->SetVisibility(false);
	}
	else
	{
		FuelLeftSeconds -= DeltaSeconds;
	}
}

void
AFirePlaceItem::CheckHasFuelStatus()
{
	if (FireWoodMesh && Inventory)
	{
		if (Inventory->HasItemOfUtility(EItemUtility::SolidFuel))
			FireWoodMesh->SetVisibility(true);
		else
			FireWoodMesh->SetVisibility(false);
	}
}

void AFirePlaceItem::SlotChanged(const FItemSlot& Slot)
{
	UE_LOG(LogTemp, Warning, TEXT("Fire place slot changed %d"), Slot.Index);
	CheckHasFuelStatus();
}

void AFirePlaceItem::SlotRemoved(const FItemSlot& Slot) {}
void AFirePlaceItem::MaxSlotsChanged(int MaxSlots) {}
