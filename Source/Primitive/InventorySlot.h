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

    UFUNCTION(BlueprintCallable) void SetItemAndCount(const FItemStruct &inItem, int inCount);
    UFUNCTION(BlueprintCallable) void SetItem(const FItemStruct &inItem);
    UFUNCTION(BlueprintCallable) void SetItemCount(int inCount);
    UFUNCTION(BlueprintCallable) void Clear();
    UFUNCTION(BlueprintCallable) FItemStruct GetItem() const { return Item; }
    UFUNCTION(BlueprintCallable) int GetItemCount() const { return ItemCount; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemSet(const FItemStruct& inItem, int inCount);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void Cleared();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SetHighlight(bool DoHighlight);

    // Drag'n'drop

    void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // Splitting and merging

    FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    bool CanMergeWith(UInventorySlot* Other) const;
    void MoveItemsHereFromSlot(UInventorySlot* Other);
    void Split();

    class UInventoryWidget* Inventory; // ???? TODO: Make sure this gets memory managed correctly

protected:

    UPROPERTY(EditAnywhere) FItemStruct Item;
    UPROPERTY(EditAnywhere) int ItemCount;
};
