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

	void Setup(UInventory* inInventory, UInventory* inEquippedItems);
	void SetupEquippedSlots();
	void SetContainerInventory(UInventory* inContainerInventory);

	// <InventoryListener>
	void SlotChanged(const FItemSlot& Slot) override;
	void SlotRemoved(const FItemSlot& Slot) override;
	void MaxSlotsChanged(int MaxSlots) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotAdded(UInventorySlot *inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotRemoved(UInventorySlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void InventorySlotsChanged();

	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> Slots;
	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> EquipmentSlots;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventorySlot> InventorySlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> EmptySlotIcon;

	UInventorySlot* AddNewSlot(UInventory *inInventory, TArray<UInventorySlot*> &ioSlots);
	void RemoveSlot(int Index);
	void DropItemsFromSlot(UInventorySlot* inSlot, int inCount);

	void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY() class UInventory *Inventory = nullptr;
	UPROPERTY() class UInventory *EquippedItems = nullptr;
	UPROPERTY() class UInventory *ContainerInventory = nullptr;

protected:

};
