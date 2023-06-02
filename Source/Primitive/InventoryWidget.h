#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainer.h"
#include "ItemStruct.h"
#include "InventorySlot.h"
#include "InventoryWidget.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable) bool AddItem(const FItemStruct& item);
	UFUNCTION(BlueprintCallable) bool RemoveItem(const FItemStruct& item);
	UFUNCTION(BlueprintCallable) void SetMaxSlots(int Count);

	// UFUNCTION(BlueprintCallable) bool MergeSlots(int SlotIndexA, int SlotIndexB);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotAdded(UInventorySlot *inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotRemoved(UInventorySlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotsChanged();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventoryItemDropped(const FItemStruct& item);

	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> Slots;
	UPROPERTY(EditAnywhere) int32 MaxSlots;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventorySlot> InventorySlotClass;

	UInventorySlot* AddToNewSlot(const FItemStruct& inItem, int inItemCount);
	void DropItemsFromSlot(UInventorySlot* inSlot, int inCount);

	void DropItem(const FItemStruct& inItem);

	void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	class APrimitiveCharacter* Player;

protected:

	bool AddToExistingSlot(const FItemStruct &inItem);
	bool RemoveFromSlot(const FItemStruct& inItem);
};
