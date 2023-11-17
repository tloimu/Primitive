// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Engine/TimerHandle.h"
#include "AnimalController.generated.h"

UCLASS()
class PRIMITIVE_API AAnimalController : public AAIController
{
	GENERATED_BODY()

public:

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Tick(float DeltaSeconds) override;

	UFUNCTION() void StartNextPatrolLeg();
	UFUNCTION() void StartNextFleeLeg();
	UFUNCTION() void StartNextApproachLeg();
	UFUNCTION() ACharacter* ChooseDetectedTarget();

	void OnCurrentMoveToLegCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);

	virtual void PlanNextMove();

	void DetectedBeing(ACharacter& inBeing);
	void LostDetectedBeing(ACharacter& inBeing);
	void AtAttackRangeToBeing(ACharacter& inBeing);
	void LostAttackRangeToBeing(ACharacter& inBeing);
	void Attack(ACharacter& inBeing);
	void HitBeing(ACharacter& inBeing);

	UPROPERTY(EditAnywhere) float PatrolRange = 2000.0f;

protected:

	FTimerHandle NextPlanTimer;

	UPROPERTY() class UAnimalMovementComponent* AnimalMovement = nullptr;
	UPROPERTY() class AAnimalCharacter* Animal = nullptr;

	UPROPERTY() TSet<ACharacter*> DetectedBeings;
	UPROPERTY() ACharacter* CurrentTargetBeing = nullptr;
	bool IsFleeing = false;
	bool IsAttacking = false;
	FVector PatrolToLocation;

	class UNavigationSystemV1* NavSystem = nullptr;
};
