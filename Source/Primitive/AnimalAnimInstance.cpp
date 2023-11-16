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

float D = 0.0f;

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

	D += DeltaSeconds;
	if (D > 0.2f)
	{
		D = 0.0f;
		if (GEngine)
		{
			FString text = FString::Printf(TEXT("Speed: %f, Turn: %f"), Speed, TurnRateYaw);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, text);
		}
	}
}
