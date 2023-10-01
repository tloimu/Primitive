#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainer.h"
#include "ItemStruct.h"
#include "InventorySlot.h"
#include "Inventory.h"
#include "Craftable.h"
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ContainerOpened(const FString &inName);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ContainerClosed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ContainerSlotAdded(UInventorySlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ContainerSlotRemoved(UInventorySlot* inSlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CraftingSlotAdded(UCrafterSlot* inSlot, const FItemStruct &inItem);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CrafterClosed();

	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> Slots;
	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> EquipmentSlots;
	UPROPERTY(EditAnywhere) TArray<UInventorySlot*> ContainerSlots;
	UPROPERTY(EditAnywhere) TArray<UCrafterSlot*> CrafterSlots;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventorySlot> InventorySlotClass;
	UPROPERTY(EditAnywhere) TSubclassOf<UCrafterSlot> CrafterSlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> EmptySlotIcon;

	UInventorySlot* MakeNewSlot(UInventory *inInventory, int SlotIndex);
	UInventorySlot* AddNewInventorySlot();
	UInventorySlot* SetNewEquipmentSlot();
	UInventorySlot* AddNewContainerSlot();
	UCrafterSlot* AddNewCrafterSlot();
	void CloseCrafter();
	void RemoveSlot(int Index);
	void DropItemsFromSlot(UInventorySlot* inSlot, int inCount);

	void SetContainer(AInteractableActor* inContainer);

	void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(BlueprintReadOnly) class UInventory *Inventory = nullptr;
	UPROPERTY(BlueprintReadOnly) class UInventory *EquippedItems = nullptr;
	UPROPERTY(BlueprintReadOnly) class UInventory *ContainerInventory = nullptr;

};
