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

    DetectedBeings.Empty();
    CurrentTargetBeing = nullptr;
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
AAnimalController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    DetectedBeings.Empty();
    CurrentTargetBeing = nullptr;
}

void
AAnimalController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // ???? TODO: e.g. when chasing, make necessary turns and attack decisions when close enough
}

void
AAnimalController::DetectedBeing(ACharacter& inBeing)
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Being at detection range [%s/%p]"), this, *inBeing.GetClass()->GetName(), &inBeing);

    DetectedBeings.Add(&inBeing);
}

void
AAnimalController::LostDetectedBeing(ACharacter& inBeing)
{
    if (CurrentTargetBeing == &inBeing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Lost target [%s/%p]"), this, *CurrentTargetBeing->GetClass()->GetName(), CurrentTargetBeing);
        IsAttacking = false;
        CurrentTargetBeing = nullptr;
    }

    DetectedBeings.Remove(&inBeing);
}

void
AAnimalController::AtAttackRangeToBeing(ACharacter& inBeing)
{
    if (CurrentTargetBeing == &inBeing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: At attacking range to target [%s/%p]"), this, *CurrentTargetBeing->GetClass()->GetName(), CurrentTargetBeing);
    }
}

void
AAnimalController::LostAttackRangeToBeing(ACharacter& inBeing)
{
    if (CurrentTargetBeing == &inBeing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Lost attack range to target [%s/%p]"), this, *CurrentTargetBeing->GetClass()->GetName(), CurrentTargetBeing);
    }
}

void
AAnimalController::Attack(ACharacter& inBeing)
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Attack [%s/%p]"), this, *inBeing.GetClass()->GetName(), &inBeing);
    IsAttacking = true;
    // ???? TODO: Start the attack move
    if (Animal)
    {
        auto dist = FVector::Dist(Animal->GetActorLocation(), inBeing.GetActorLocation());
        float HitDistance = 200.0f;
        if (dist <= HitDistance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Attack hit being [%s/%p]"), this, *inBeing.GetClass()->GetName(), &inBeing);
            GetWorldTimerManager().SetTimer(NextPlanTimer, this, &AAnimalController::PlanNextMove, 3.0f);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Attack missed being [%s/%p] distance %f"), this, *inBeing.GetClass()->GetName(), &inBeing, dist);
            GetWorldTimerManager().SetTimer(NextPlanTimer, this, &AAnimalController::PlanNextMove, 3.0f);
        }
    }
}

void
AAnimalController::HitBeing(ACharacter& inBeing)
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Hit [%s/%p]"), this, *inBeing.GetClass()->GetName(), &inBeing);
    // ???? TODO: Deal damage if possible
    if (IsAttacking)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Damage being [%s/%p]"), this, *inBeing.GetClass()->GetName(), &inBeing);
        //GetWorldTimerManager().SetTimer(NextPlanTimer, this, &AAnimalController::PlanNextMove, 3.0f);
    }
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

    if (CurrentTargetBeing)
    {
        if (IsFleeing)
            StartNextFleeLeg();
        else
            StartNextApproachLeg();
    }
    else
    {
        CurrentTargetBeing = ChooseDetectedTarget();
        if (CurrentTargetBeing)
        {
            StartNextApproachLeg();
        }
        else
        {
            StartNextPatrolLeg();
        }
    }
}

ACharacter*
AAnimalController::ChooseDetectedTarget()
{
    for (auto being : DetectedBeings)
    {
        // if (GetClass() != being->GetClass())
        {
            UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Chose target [%s/%p]"), this, *being->GetClass()->GetName(), being);
            return being;
        }
    }
    return nullptr;
}


void
AAnimalController::StartNextFleeLeg()
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: StartNextFleeLeg"), this);
    if (AnimalMovement && CurrentTargetBeing)
    {
        float CheckFleeDirectionDistance = 500.0f; // ???? Maybe checking time interval would be better?
        auto awayVector = GetPawn()->GetActorLocation() - CurrentTargetBeing->GetActorLocation();
        float distance = awayVector.Length();
        awayVector.Normalize();

        MoveToLocation(GetPawn()->GetActorLocation() + awayVector * CheckFleeDirectionDistance);
        if (distance < CheckFleeDirectionDistance * 2)
            AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Sprint);
        else
            AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Run);
    }
}

void
AAnimalController::StartNextApproachLeg()
{
    UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: StartNextApproachLeg"), this);
    if (Animal && AnimalMovement && CurrentTargetBeing)
    {
        float AttackDistance = 100.0f;
        if (FVector::Dist(Animal->GetActorLocation(), CurrentTargetBeing->GetActorLocation()) <= AttackDistance*2)
        {
            UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Approached already - attacking"), this);
            Attack(*CurrentTargetBeing);
            return;
        }
        else
        {
            auto result = MoveToActor(CurrentTargetBeing, AttackDistance);
            if (result == EPathFollowingRequestResult::AlreadyAtGoal)
            {
                UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Approach goal reached - attacking"), this);
                Attack(*CurrentTargetBeing);
            }
            else if (result == EPathFollowingRequestResult::Failed)
            {
                UE_LOG(LogTemp, Warning, TEXT("Animal [%p]: Approach goal failed - cancelling approach"), this);
                CurrentTargetBeing = nullptr;
                IsAttacking = false;
                //GetWorldTimerManager().SetTimer(NextPlanTimer, this, &AAnimalController::PlanNextMove, 3.0f);
            }
            else
            {
                AnimalMovement->SetAnimalMovementMode(EAnimalMovementMode::Run);
            }
        }
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
