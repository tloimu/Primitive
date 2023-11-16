// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimalAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly) double Speed;
	UPROPERTY(BlueprintReadOnly) double TurnRateYaw;

protected:

	void NativeBeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY() class UCharacterMovementComponent* CharacterMovement;
	UPROPERTY() class AAnimalController* CharacterController;

private:

	FRotator	LastKnownDirection;
};
