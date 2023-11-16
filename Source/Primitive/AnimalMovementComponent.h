// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UAnimalMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	//using UCharacterMovementComponent::UCharacterMovementComponent;
	UAnimalMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unused2") bool Unused2;
};
