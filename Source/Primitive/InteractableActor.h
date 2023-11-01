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

    bool Interact_Implementation() override;
    bool Hit_Implementation() override;
    void Consume_Implementation() override;
    /*
    UFUNCTION(BlueprintCallable) UItemSpecs* GetItemSpecs() const;
    UFUNCTION(BlueprintCallable) void SetItemSpecs(UItemSpecs* Item);
    */

    UFUNCTION(BlueprintCallable) FItemStruct GetItem() const { return Item; }
    UFUNCTION(BlueprintCallable) bool IsEmpty() const;
    UFUNCTION(BlueprintCallable) bool CanBePicked() const;

    UPROPERTY(EditAnywhere) FItemStruct Item;

    UPROPERTY(BlueprintReadOnly) class UInventory* Inventory = nullptr;
    UPROPERTY(BlueprintReadOnly) class UCrafter* Crafter = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FBuildSnapBox> BuildSnapBoxes;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool IsFoundation = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool RequireFoundation = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool RequireSnapBox = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 CurrentState = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FTransform OriginalStateTransform;
    UPROPERTY(BlueprintReadOnly) AInteractableActor* SupportedByItem = nullptr;
    UPROPERTY(BlueprintReadOnly) TArray<AInteractableActor*> SupportsItems;

    void AddOnItem(AInteractableActor& inSupport);
    void DestroyItem(); // destroys also what ever this item supports

    virtual void OnLoaded() {}

    float GetClockSpeed() const;
};
