// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalCharacter.h"
#include "AnimalMovementComponent.h"
#include "Components/SphereComponent.h"

AAnimalCharacter::AAnimalCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAnimalMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AnimalMovement = Cast<UAnimalMovementComponent>(GetCharacterMovement());

	PrimaryActorTick.bCanEverTick = true;

	// Make animations nicer
	bUseControllerRotationYaw = false;

	BeingDetectionRangeCollision =
		CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	BeingDetectionRangeCollision->SetupAttachment(RootComponent);

	BeingAttackRangeCollision =
		CreateDefaultSubobject<USphereComponent>(TEXT("Player Attack Collision Detection"));
	BeingAttackRangeCollision->SetupAttachment(RootComponent);

	DamageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//DamageCollision->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));
}

// Called when the game starts or when spawned
void
AAnimalCharacter::BeginPlay()
{
	Super::BeginPlay();

	mController = Cast<AAnimalController>(GetController());

	if (ensure(BeingDetectionRangeCollision))
	{
		BeingDetectionRangeCollision->OnComponentBeginOverlap.AddDynamic(this,
			&AAnimalCharacter::OnBeingDetectedOverlapBegin);

		BeingDetectionRangeCollision->OnComponentEndOverlap.AddDynamic(this,
			&AAnimalCharacter::OnBeingDetectedOverlapEnd);
	}

	if (ensure(BeingAttackRangeCollision))
	{
		BeingAttackRangeCollision->OnComponentBeginOverlap.AddDynamic(this,
			&AAnimalCharacter::OnBeingAttackOverlapBegin);

		BeingAttackRangeCollision->OnComponentEndOverlap.AddDynamic(this,
			&AAnimalCharacter::OnBeingAttackOverlapEnd);
	}

	if (ensure(DamageCollision))
	{
		DamageCollision->OnComponentBeginOverlap.AddDynamic(this,
			&AAnimalCharacter::OnDealDamageOverlapBegin);
	}
}

// Called every frame
void
AAnimalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void
AAnimalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void
AAnimalCharacter::OnBeingDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	auto Being = Cast<ACharacter>(OtherActor);
	if (Being && this != Being)
	{
		if (mController)
		{
			mController->DetectedBeing(*Being);
		}
	}
}

void
AAnimalCharacter::OnBeingDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Being = Cast<ACharacter>(OtherActor);
	if (Being && this != Being)
	{
		if (mController)
		{
			mController->LostDetectedBeing(*Being);
		}
	}
}

void
AAnimalCharacter::OnBeingAttackOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	auto Being = Cast<ACharacter>(OtherActor);
	if (Being && this != Being)
	{
		if (mController)
		{
			mController->AtAttackRangeToBeing(*Being);
		}
	}
}

void
AAnimalCharacter::OnBeingAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Being = Cast<ACharacter>(OtherActor);
	if (Being && this != Being)
	{
		if (mController)
		{
			mController->LostAttackRangeToBeing(*Being);
		}
	}
}

void
AAnimalCharacter::OnDealDamageOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Being = Cast<ACharacter>(OtherActor);
	if (Being && this != Being)
	{
		if (mController)
		{
			mController->HitBeing(*Being);
		}
	}
}
