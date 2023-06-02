#include "InventoryWidget.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
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
		auto slot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
		if (slot)
		{
			slot->SetItemAndCount(inItem, 1);
			Slots.Add(slot);
			InventorySlotAdded(slot);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to create inventory slot due missing InventorySlotClass"));
			return false;
		}
		return false;
	}

	return false;
}

bool UInventoryWidget::RemoveItem(const FItemStruct& inItem)
{
	return RemoveFromSlot(inItem);
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
