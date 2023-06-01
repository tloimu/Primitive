#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "ItemStruct.h"
#include "InteractableActor.generated.h"

UCLASS(Blueprintable)
class AInteractableActor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:

    void Interact_Implementation() override;
    void Hit_Implementation() override;
    void Consume_Implementation() override;
    /*
    UFUNCTION(BlueprintCallable) UItemSpecs* GetItemSpecs() const;
    UFUNCTION(BlueprintCallable) void SetItemSpecs(UItemSpecs* Item);
    */

    UFUNCTION(BlueprintCallable) FItemStruct GetItem() const { return Item; }

protected:

    UPROPERTY(EditAnywhere) FItemStruct Item;

    //UPROPERTY(EditAnywhere) UItemSpecs* ItemSpecs;
};
