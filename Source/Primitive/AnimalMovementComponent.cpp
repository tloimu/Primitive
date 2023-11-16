// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalMovementComponent.h"

UAnimalMovementComponent::UAnimalMovementComponent(const FObjectInitializer& ObjectInitializer) : UCharacterMovementComponent(ObjectInitializer)
{
	SetAnimalMovementMode(EAnimalMovementMode::None);
	bUseControllerDesiredRotation = true;
}

void
UAnimalMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	switch (DesiredMovementMode)
	{
	case EAnimalMovementMode::Stalk:
		SetVelocityAndTurnRate(Stalk.Speed, Stalk.TurnRate.Yaw);
		break;
	case EAnimalMovementMode::Walk:
		SetVelocityAndTurnRate(Walk.Speed, Walk.TurnRate.Yaw);
		break;
	case EAnimalMovementMode::Trot:
		SetVelocityAndTurnRate(Trot.Speed, Trot.TurnRate.Yaw);
		break;
	case EAnimalMovementMode::Run:
		SetVelocityAndTurnRate(Run.Speed, Run.TurnRate.Yaw);
		break;
	case EAnimalMovementMode::Sprint:
		SetVelocityAndTurnRate(Sprint.Speed, Sprint.TurnRate.Yaw);
		break;
	default:
		SetVelocityAndTurnRate(0.0f, 0.0f);
		break;
	}
}


void UAnimalMovementComponent::SetVelocityAndTurnRate(double Speed, double TurnRateYaw)
{
	MaxWalkSpeed = Speed;
	RotationRate.Yaw = TurnRateYaw;
}

void
UAnimalMovementComponent::GoSleep()
{
	SetAnimalMovementMode(EAnimalMovementMode::None);
}

void
UAnimalMovementComponent::GoIdle()
{
	SetAnimalMovementMode(EAnimalMovementMode::None);
}

void
UAnimalMovementComponent::SetAnimalMovementMode(EAnimalMovementMode Mode)
{
	DesiredMovementMode = Mode;
}
