// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalCharacter.h"
#include "AnimalMovementComponent.h"

AAnimalCharacter::AAnimalCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Make animations nicer
	bUseControllerRotationYaw = false;
	auto mc = GetCharacterMovement();
	if (mc)
	{
		mc->bUseControllerDesiredRotation = true;
		mc->RotationRate.Yaw = 92.0f;
	}
}

void AAnimalCharacter::SetVelocity(double Speed)
{
	auto mc = GetCharacterMovement();
	if (mc)
	{
		mc->MaxWalkSpeed = Speed;
	}
}

void AAnimalCharacter::MoveBySlowlyWalking()
{
	SetVelocity(85.0f);
	auto mc = GetCharacterMovement();
	if (mc)
	{
		mc->RotationRate.Yaw = 90.0f;
	}
}

void AAnimalCharacter::MoveByTrotting()
{
	SetVelocity(1400.0f);
}

void AAnimalCharacter::MoveByRunning()
{
	SetVelocity(11000.0f);
}

void AAnimalCharacter::MoveByTopSpeed()
{
	SetVelocity(16000.0f);
}

// Called when the game starts or when spawned
void AAnimalCharacter::BeginPlay()
{
	Super::BeginPlay();

	mController = Cast<AAnimalController>(GetController());
	
}

// Called every frame
void AAnimalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAnimalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

