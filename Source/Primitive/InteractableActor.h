#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractableActor.generated.h"

UCLASS(Blueprintable)
class AInteractableActor : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    void Interact_Implementation() override { };
    void Hit_Implementation() override { };
    void Consume_Implementation() override { };
};
