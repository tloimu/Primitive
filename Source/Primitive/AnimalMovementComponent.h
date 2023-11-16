// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalMovementComponent.generated.h"


UENUM(BlueprintType)
enum class EAnimalMovementMode : uint8
{
	None = 0    UMETA(DisplayName = "None"),
	Stalk = 1   UMETA(DisplayName = "Stalk"),
	Walk = 2	UMETA(DisplayName = "Walk"),
	Trot = 3    UMETA(DisplayName = "Trot"),
	Run = 4     UMETA(DisplayName = "Run"),
	Sprint = 5  UMETA(DisplayName = "Sprint")
};


USTRUCT(BlueprintType)
struct FAnimalMovementParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) double Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator TurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) double MaxDuration = -1.0f; // negative value means indefinite
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EAnimalMovementMode Mode = EAnimalMovementMode::None;
};

UCLASS()
class PRIMITIVE_API UAnimalMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	//using UCharacterMovementComponent::UCharacterMovementComponent;
	UAnimalMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	void GoSleep();
	void GoIdle();

	void SetAnimalMovementMode(EAnimalMovementMode Mode);
	void SetVelocityAndTurnRate(double Speed, double TurnRateYaw);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAnimalMovementParams Stalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAnimalMovementParams Walk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAnimalMovementParams Trot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAnimalMovementParams Run;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAnimalMovementParams Sprint;

protected:

	EAnimalMovementMode DesiredMovementMode = EAnimalMovementMode::None;
};
