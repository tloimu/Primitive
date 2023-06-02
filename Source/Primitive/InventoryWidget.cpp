#include "InventoryWidget.h"
#include "InventorySlotDragOperation.h"
#include "InteractableActor.h"
#include "PrimitiveCharacter.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
	Player = nullptr;
	bIsFocusable = true;
	MaxSlots = 30;
	InventorySlotClass = StaticClass();
}

void UInventoryWidget::SetMaxSlots(int Count)
{
	MaxSlots = Count;
	InventorySlotsChanged();
}

bool UInventoryWidget::AddItem(const FItemStruct& inItem)
{
	if (AddToExistingSlot(inItem))
	{
		return true;
	}

	if (Slots.Num() < MaxSlots)
	{
		auto slot = AddToNewSlot(inItem, 1);
		if (slot)
			return true;
		else
			return false;
	}

	return false;
}

bool UInventoryWidget::RemoveItem(const FItemStruct& inItem)
{
	return RemoveFromSlot(inItem);
}

UInventorySlot*
UInventoryWidget::AddToNewSlot(const FItemStruct& inItem, int inItemCount)
{
	auto slot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
	if (slot)
	{
		slot->Inventory = this;
		slot->SetItemAndCount(inItem, inItemCount);
		Slots.Add(slot);
		InventorySlotAdded(slot);
		return slot;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create inventory slot due missing InventorySlotClass"));
		return nullptr;
	}
}

bool UInventoryWidget::AddToExistingSlot(const FItemStruct& inItem)
{
	for (auto slot : Slots)
	{
		auto slotItem = slot->GetItem();
		if (slotItem.Id == inItem.Id)
		{
			auto slotItemCount = slot->GetItemCount();
			if (slotItemCount >= 0 && slotItemCount < slotItem.MaxStackSize)
			{
				slot->SetItemCount(slotItemCount + 1);
				if (slotItemCount == 0)
					slot->Clear();
				return true;
			}
		}
	}

	return false;
}

bool UInventoryWidget::RemoveFromSlot(const FItemStruct& inItem)
{
	for (auto slot : Slots)
	{
		auto slotItem = slot->GetItem();
		auto slotItemCount = slot->GetItemCount();
		if (slotItem.Id == inItem.Id)
		{
			if (slotItemCount > 1)
				slot->SetItemCount(slotItemCount - 1);
			else
			{
				slot->Clear();
				InventorySlotRemoved(slot);
				Slots.RemoveSingle(slot);
			}
			return true;
		}
	}
	return false;
}


void UInventoryWidget::InventorySlotAdded_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Added slot to Inventory"));
}

void UInventoryWidget::InventorySlotRemoved_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Removed slot from Inventory"));
}

void UInventoryWidget::InventorySlotsChanged_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SlotsChanged count %d"), Slots.Num());
}

void UInventoryWidget::InventoryItemDropped_Implementation(const FItemStruct& item)
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory item dropped %s"), *item.Name);
}

// Drag'n'Drop

void UInventoryWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag enter"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from %s"), *dragged->GetItem().Name);
	}
}

void UInventoryWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag leave"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from %s"), *dragged->GetItem().Name);
	}
}

bool UInventoryWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag over"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from %s"), *dragged->GetItem().Name);
	}
	return true;
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: OnDrop"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		auto n = dragged->GetItemCount();
		if (InDragDropEvent.GetModifierKeys().IsShiftDown())
			n = 1;
		DropItemsFromSlot(dragged, n);
	}
	return true;
}


void UInventoryWidget::DropItemsFromSlot(UInventorySlot* inSlot, int inCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory: Dropped %d item %s"), inCount, *inSlot->GetItem().Name);
	auto count = inCount;
	if (inCount >= inSlot->GetItemCount())
	{
		Slots.RemoveSingle(inSlot);
		InventorySlotRemoved(inSlot);
		count = inSlot->GetItemCount();
	}
	else
	{
		inSlot->SetItemCount(inSlot->GetItemCount() - inCount);
	}

	for (int i = 0; i < count; i++)
	{
		DropItem(inSlot->GetItem());
	}
}

void UInventoryWidget::DropItem(const FItemStruct& inItem)
{
	if (Player)
		Player->CreateDroppedItem(inItem);
	InventoryItemDropped(inItem);
}
