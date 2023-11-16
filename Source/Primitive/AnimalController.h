// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AnimalController.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API AAnimalController : public AAIController
{
	GENERATED_BODY()

public:

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	UFUNCTION() void StartNextPatrolLeg();
	UFUNCTION() void StartNextFleeLeg();
	UFUNCTION() void StartNextApproachLeg();

	void OnCurrentMoveToLegCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);

	virtual void PlanNextMove();

	UPROPERTY(EditAnywhere) float PatrolRange = 5000.0f;

protected:

	UPROPERTY() class UAnimalMovementComponent* AnimalMovement;
	UPROPERTY() class AAnimalCharacter* Animal;

	UPROPERTY() AActor* CurrentTargetActor = nullptr;
	bool IsFleeing = false;
	bool IsAttacking = false;
	FVector PatrolToLocation;

	class UNavigationSystemV1* NavSystem = nullptr;
};
