#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "ItemStruct.h"
#include "Inventory.h"
#include "Craftable.h"
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
    UFUNCTION(BlueprintCallable) bool IsEmpty() const;
    UFUNCTION(BlueprintCallable) bool CanBePicked() const;

    UPROPERTY(EditAnywhere) FItemStruct Item;

    UPROPERTY(EditAnywhere, BlueprintReadOnly) class UInventory* Inventory = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) class UCrafter* Crafter = nullptr;
};
