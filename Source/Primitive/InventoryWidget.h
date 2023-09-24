#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainer.h"
#include "ItemStruct.h"
#include "InventorySlot.h"
#include "Inventory.h"
#include "InventoryWidget.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UInventoryWidget : public UUserWidget, public IInventoryListener
{
	GENERATED_BODY()

public:

	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	// <InventoryListener>
	void SlotChanged(int Index, const FItemSlot& Slot) override;
	void MaxSlotsChanged(int MaxSlots) override;
	void SlotRemoved(int Index) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotAdded(UInventorySlot *inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotRemoved(UInventorySlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotsChanged();

	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> Slots;
//	UPROPERTY(EditAnywhere) int32 MaxSlots;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventorySlot> InventorySlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> EmptySlotIcon;

	//UInventorySlot* AddToNewSlot(const FItemStruct& inItem, int inItemCount);
	UInventorySlot* AddToNewSlot(const FItemSlot& inSlot);
	void RemoveSlot(int Index);
	void DropItemsFromSlot(UInventorySlot* inSlot, int inCount);

	void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY() class UInventory *Inventory = nullptr;

protected:

};
