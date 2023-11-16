// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalAnimInstance.h"
#include "AnimalCharacter.h"
#include "AnimalController.h"
#include "GameFramework/CharacterMovementComponent.h"

void
UAnimalAnimInstance::NativeBeginPlay()
{
}

void
UAnimalAnimInstance::NativeInitializeAnimation()
{
	auto character = Cast<AAnimalCharacter>(GetOwningActor());
	if (character)
	{
		CharacterMovement = character->GetCharacterMovement();
		CharacterController = character->GetController<AAnimalController>();
	}
}

void
UAnimalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (CharacterMovement)
	{
		Speed = CharacterMovement->Velocity.Length();
		auto currentDir = CharacterMovement->GetActorTransform().GetRotation().Rotator();
		auto tr = (currentDir - LastKnownDirection);
		tr.Normalize();
		TurnRateYaw = tr.Yaw / DeltaSeconds;
		LastKnownDirection = currentDir;
	}
}
