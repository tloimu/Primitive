// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnimalController.h"
#include "AnimalCharacter.generated.h"

UCLASS()
class PRIMITIVE_API AAnimalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAnimalCharacter(const FObjectInitializer &ObjectInitializer);

	void MoveBySlowlyWalking();
	void MoveByTrotting();
	void MoveByRunning();
	void MoveByTopSpeed();

	void SetVelocity(double Speed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AAnimalController* mController = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
