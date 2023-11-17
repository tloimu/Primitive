// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalController.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "AnimalController.h"
#include "AnimalCharacter.h"
#include "AnimalMovementComponent.h"


void
AAnimalController::BeginPlay()
{
    Super::BeginPlay();

    CurrentTargetActor = nullptr;
    NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
    Animal = GetPawn<AAnimalCharacter>();
    if (Animal)
    {
        AnimalMovement = Cast<UAnimalMovementComponent>(Animal->GetMovementComponent());
        if (!AnimalMovement)
        {
            UE_LOG(LogTemp, Error, TEXT("Animal [%p]: No AnimalMovement found at BeginPlay"), this);
        }
    }
    GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AAnimalController::OnCurrentMoveToLegCompleted);

    PlanNextMove();
}

void
AAnimalController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // ???? TODO: e.g. when chasing, make necessary turns and attack decisions when close enough
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
        if (IsFleeing)
            StartNextFleeLeg();
        else if (IsAttacking)
            StartNextApproachLeg();
    }
    else
    {
        StartNextPatrolLeg();
    }
}
void
AAnimalController::StartNextFleeLeg()
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: StartNextFleeLeg"), this);
    if (AnimalMovement)
    {
        StartNextPatrolLeg();    // ???? TODO:
        AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Sprint);
    }
}

void
AAnimalController::StartNextApproachLeg()
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: StartNextApproachLeg"), this);
    if (AnimalMovement)
    {
        StartNextPatrolLeg();    // ???? TODO:
        AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Sprint);
    }
}

void
AAnimalController::StartNextPatrolLeg()
{
    if (NavSystem)
    {
        FVector loc;
        FVector forward = GetPawn()->GetActorForwardVector();
        forward.Z = 0.0f;
        FVector fromLoc = GetPawn()->GetActorLocation() + forward * (PatrolRange + 200.0f);
        if (NavSystem->K2_GetRandomLocationInNavigableRadius(GetWorld(), fromLoc, loc, PatrolRange))
        {
            if (PatrolToLocation != loc)
            {
                PatrolToLocation = loc;
                MoveToLocation(PatrolToLocation, 150.0f);
                if (AnimalMovement)
                {
                    auto mode = FMath::RandRange(0, 1);
                    if (mode == 0)
                        AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Walk);
                    else
                        AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Trot);
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
