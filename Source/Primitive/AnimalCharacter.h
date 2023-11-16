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

protected:
	AAnimalController* mController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) class UAnimalMovementComponent* AnimalMovement;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
