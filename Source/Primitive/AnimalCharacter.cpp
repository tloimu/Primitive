// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalCharacter.h"
#include "AnimalMovementComponent.h"

AAnimalCharacter::AAnimalCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAnimalMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AnimalMovement = Cast<UAnimalMovementComponent>(GetCharacterMovement());

	PrimaryActorTick.bCanEverTick = true;

	// Make animations nicer
	bUseControllerRotationYaw = false;
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

