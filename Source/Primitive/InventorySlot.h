#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemStruct.h"
#include "InventorySlot.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UInventorySlot : public UUserWidget
{
    GENERATED_BODY()

public:
    UInventorySlot(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable) void SetItem(const FItemStruct &inItem);
    UFUNCTION(BlueprintCallable) void SetItemCount(int inCount);
    UFUNCTION(BlueprintCallable) void Clear();
    UFUNCTION(BlueprintCallable) FItemStruct GetItem() const { return Item; }
    UFUNCTION(BlueprintCallable) int GetItemCount() const { return ItemCount; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemSet(const FItemStruct& inItem, int inCount);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void Cleared();

protected:

    UPROPERTY(EditAnywhere) FItemStruct Item;
    UPROPERTY(EditAnywhere) int ItemCount;
};
