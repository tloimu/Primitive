// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "NiagaraComponent.h"
#include "FirePlaceItem.generated.h"


UCLASS()
class PRIMITIVE_API AFirePlaceItem : public AInteractableActor
{
	GENERATED_BODY()

public:

	AFirePlaceItem(const FObjectInitializer &Initializer);

	bool Interact_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Fire Place Item") UNiagaraSystem* FireEffectSystem = nullptr;
	UPROPERTY() UNiagaraComponent * FireEffect = nullptr;

	void Tick(float DeltaSeconds) override;

	void OnLoaded() override;

	void CheckFireEffect();
	void HeatItems(float DeltaSeconds);
	void CheckFuel(float DeltaSeconds);

	float FuelLeftSeconds = 0.0f;
};
