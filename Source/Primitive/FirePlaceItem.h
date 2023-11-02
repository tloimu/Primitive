// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "NiagaraComponent.h"
#include "FirePlaceItem.generated.h"


UCLASS()
class PRIMITIVE_API AFirePlaceItem : public AInteractableActor, public IInventoryListener
{
	GENERATED_BODY()

public:

	AFirePlaceItem(const FObjectInitializer &Initializer);

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SlotChanged(const FItemSlot& Slot) override;
	void SlotRemoved(const FItemSlot& Slot) override;
	void MaxSlotsChanged(int MaxSlots) override;

	bool Interact_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Fire Place Item") UNiagaraSystem* FireEffectSystem = nullptr;
	UPROPERTY() UNiagaraComponent * FireEffect = nullptr;
	UPROPERTY(EditAnywhere, Category = "Fire Place Item") FString FireWoodMeshName;
	UPROPERTY() UStaticMeshComponent* FireWoodMesh = nullptr;

	void Tick(float DeltaSeconds) override;

	void OnLoaded() override;

	void CheckFireEffect();
	void HeatItems(float DeltaSeconds);
	void CheckFuel(float DeltaSeconds);
	void CheckHasFuelStatus();

	float FuelLeftSeconds = 0.0f;
};
