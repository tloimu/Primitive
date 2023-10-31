#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.h"
#include "InventorySlotDragOperation.h"
#include "InventorySlot.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UInventorySlot : public UUserWidget
{
    GENERATED_BODY()

public:
    UInventorySlot(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotSet(const FItemSlot& inSlot);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotRemoved(const FItemSlot& inSlot);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SetHighlight(bool DoHighlight);

    UFUNCTION(BlueprintCallable) bool IsEmpty();
    UFUNCTION(BlueprintCallable) int GetItemCount();
    UFUNCTION(BlueprintCallable) UTexture* GetIcon();

    // Drag'n'drop

    void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // Splitting and merging

    FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    UPROPERTY() class UInventory* Inventory = nullptr;
    UPROPERTY() int SlotIndex = 0;

    UPROPERTY(EditAnywhere) EBodyPart EquippedIn = EBodyPart::None; // ???? TODO: Move this to new <UEquipmentSlot> class

    UPROPERTY(EditAnywhere) TSubclassOf<UDraggedInventorySlot> DraggedInventorySlotWidgetClass;

protected:
};
