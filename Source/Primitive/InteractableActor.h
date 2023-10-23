#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "ItemStruct.h"
#include "Inventory.h"
#include "Craftable.h"
#include "Components/BoxComponent.h"
#include "InteractableActor.generated.h"



USTRUCT(BlueprintType)
struct FBuildSnapBox
{
    GENERATED_BODY()

    FBuildSnapBox() {}

    bool operator== (const FBuildSnapBox rhs) const
    {
        if (Box == rhs.Box)
        {
            return true;
        }
        else return false;
    }


    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool IsStackingUp = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) UBoxComponent* Box;
};



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
    UPROPERTY(EditAnywhere, BlueprintReadOnly) class UCrafter* Crafter = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FBuildSnapBox> BuildSnapBoxes;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool IsFoundation = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool RequireFoundation = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool RequireSnapBox = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) AInteractableActor* SupportedByItem = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<AInteractableActor*> SupportsItems;

    void AddOnItem(AInteractableActor& inSupport);
    void DestroyItem(); // destroys also what ever this item supports
};
