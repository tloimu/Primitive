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

	UPROPERTY(EditAnywhere) class UShapeComponent* BeingDetectionRangeCollision;
	UPROPERTY(EditAnywhere) class UShapeComponent* BeingAttackRangeCollision;
	UPROPERTY(EditAnywhere) class UShapeComponent* DamageCollision;

protected:
	AAnimalController* mController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) class UAnimalMovementComponent* AnimalMovement;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Detection, chase and attack

	UFUNCTION()
	void OnBeingDetectedOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBeingDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnBeingAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBeingAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnDealDamageOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
