// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalController.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "AnimalController.h"
#include "AnimalCharacter.h"


void
AAnimalController::BeginPlay()
{
    Super::BeginPlay();

    CurrentTargetActor = nullptr;
    NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
    Animal = GetPawn<AAnimalCharacter>();
    GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AAnimalController::OnCurrentMoveToLegCompleted);

    PlanNextMove();
}

void
AAnimalController::OnCurrentMoveToLegCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Move leg completed"), this);
    PlanNextMove();
}

void
AAnimalController::PlanNextMove()
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: PlanNextMove"), this);

    if (CurrentTargetActor)
    {
        // ???? TODO: Chase or attack target actor
    }
    else
    {
        StartNextPatrolLeg();
    }
}

void
AAnimalController::StartNextPatrolLeg()
{
    if (NavSystem)
    {
        FVector loc;
        if (NavSystem->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(), loc, PatrolRange))
        {
            if (PatrolToLocation != loc)
            {
                PatrolToLocation = loc;
                MoveToLocation(PatrolToLocation, 150.0f);
                if (Animal)
                {
                    auto mode = FMath::RandRange(0, 0);
                    if (mode == 0)
                        Animal->MoveBySlowlyWalking();
                    else
                        Animal->MoveByTrotting();
                }
                UE_LOG(LogTemp, Warning, TEXT("Animal: Moving to [%f, %f, %f]"), PatrolToLocation.X, PatrolToLocation.Y, PatrolToLocation.Z);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Animal: Moving to [%f, %f, %f] where already was"), PatrolToLocation.X, PatrolToLocation.Y, PatrolToLocation.Z);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Animal: Moving to failed"));
        }
    }
}
