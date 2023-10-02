// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "PrimitiveCharacter.h"

UInventory::UInventory(): UObject(), MaxSlots(1)
{
}

UInventory::UInventory(const FObjectInitializer& ObjectInitializer): UObject(ObjectInitializer), MaxSlots(1)
{
}

FItemSlot&
UInventory::GetSlotAt(int Index)
{
	if (Index >= 0 && Index < Slots.Num())
		return Slots[Index];
	else
		return NoneSlot;
}

const FItemStruct*
UInventory::FindItem(const FString& inId) const
{
	if (Player)
	{
		return Player->FindItem(inId);
	}
	return nullptr;
}

bool
UInventory::CanMergeWith(FItemSlot& ToSlot, FItemSlot& FromSlot) const
{
	if (ToSlot.Count == 0)
	{
		if (ToSlot.CanOnlyWearIn.IsEmpty())
			return true;

		for (auto bp : FromSlot.Item.CanWearIn)
		{
			UE_LOG(LogTemp, Warning, TEXT("FromSlot CanWear in %d"), bp);
		}
		for (auto bp : ToSlot.CanOnlyWearIn)
		{
			UE_LOG(LogTemp, Warning, TEXT("ToSlot CanWear in %d"), bp);
		}

		if (!ToSlot.CanOnlyWearIn.Intersect(FromSlot.Item.CanWearIn).IsEmpty())
			return true;
	}
	else
	{
		if (ToSlot.Item == FromSlot.Item)
			return true;
	}

	return false;
}

void
UInventory::MergeWith(FItemSlot& ToSlot, FItemSlot& FromSlot, int Count)
{
	UE_LOG(LogTemp, Warning, TEXT("Merge from slot %d to %d items %d"), FromSlot.Index, ToSlot.Index, Count);
	if (CanMergeWith(ToSlot, FromSlot))
	{
		auto n = FMath::Min(FromSlot.Item.MaxStackSize - ToSlot.Count, Count);
		ToSlot.Item = FromSlot.Item;
		ToSlot.Count += n;
		FromSlot.Count -= n;
		if (FromSlot.Count == 0)
			FromSlot.Item = FItemStruct();
		if (InventoryListener)
		{
			InventoryListener->SlotChanged(FromSlot);
			InventoryListener->SlotChanged(ToSlot);
		}
	}
}


bool
UInventory::AddItem(const FItemStruct& item, int count)
{
	// First, find slot that already has the same stuff and has room left
	UE_LOG(LogTemp, Warning, TEXT("Adding %d item %s to slot (slots=%d) icon=%s"), count, *item.ItemClass.Get()->GetName(), Slots.Num(), *item.Icon.GetAssetName());

	// ???? TODO: Fix to allow spreading items into multiple slots if they do not fit into one

	for (int i = 0; i < Slots.Num(); i++)
	{
		auto& slot = Slots[i];
		if (!slot.Inventory)
			UE_LOG(LogTemp, Error, TEXT("NULL inventory on slot %d"), i);
		int fitsInSlot = FMath::Min(item.MaxStackSize - slot.Count, count);
		if (slot.Item == item && fitsInSlot > 0)
		{
			slot.Count += fitsInSlot;
			count -= fitsInSlot;
			if (InventoryListener)
				InventoryListener->SlotChanged(slot);
			if (count == 0)
				return true;
		}
	}

	// Then, find an empty slot
	for (int i = 0; i < Slots.Num(); i++)
	{
		auto& slot = Slots[i];
		int fitsInSlot = FMath::Min(item.MaxStackSize - slot.Count, count);
		if (slot.Count == 0 && fitsInSlot > 0)
		{
			slot.Item = item;
			slot.Count += fitsInSlot;
			count -= fitsInSlot;
			if (InventoryListener)
				InventoryListener->SlotChanged(slot);
			if (count == 0)
				return true;
		}
	}

	return false;
}

bool
UInventory::RemoveItem(const FItemStruct& item, int count)
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		auto& slot = Slots[i];
		if (slot.Count > 0 && slot.Item == item)
		{
			slot.Count--;
			if (InventoryListener)
				InventoryListener->SlotChanged(slot);
		}
	}
	return false;
}

bool
UInventory::SetMaxSlots(int Count)
{
/*
	if (Count < Slots.Num())
	{
		// Cancel if too much stuff
		Organize();
		for (int i = Count; i < Slots.Num(); i++)
		{
			if (Slots[i].Count > 0)
				return false;
		}
	}
*/

	MaxSlots = Count;
	while (Slots.Num() < MaxSlots)
	{
		FItemSlot empty;
		empty.Index = Slots.Num();
		empty.Inventory = this;
		Slots.Add(empty);
	}

	if (InventoryListener)
		InventoryListener->MaxSlotsChanged(Count);
	return true;
}

void
UInventory::Organize()
{
	// ???? TODO: Combine stacks and take into account diffent quality items being different
	TArray<FItemSlot> newSlots;
	TMap<FString, int> items;
	for (auto& slot : Slots)
	{
		if (slot.Count > 0)
		{
			newSlots.Add(slot);
		}
	}

	FItemSlot empty;
	while (newSlots.Num() < Slots.Num())
	{
		empty.Index = newSlots.Num();
		newSlots.Add(empty);
	}

	Slots = newSlots;

	if (InventoryListener)
	{
		for (int i = 0; i < Slots.Num(); i++)
			InventoryListener->SlotChanged(Slots[i]);
	}

}

bool
UInventory::IsEmpty() const
{
	for (auto& slot : Slots)
	{
		if (slot.Count > 0)
			return false;
	}
	return true;
}



void
UInventory::SplitSlot(int Index)
{
	UE_LOG(LogTemp, Warning, TEXT("Split slot index %d"), Index);

	if (Index < Slots.Num())
	{
		auto& inSlot = Slots[Index];
		if (inSlot.Count > 1)
		{
			auto n = inSlot.Count / 2;
			for (int i = 0; i < Slots.Num(); i++)
			{
				auto& slot = Slots[i];
				if (slot.Count == 0)
				{
					slot.Item = inSlot.Item;
					slot.Count = n;
					inSlot.Count -= n;
					if (InventoryListener)
					{
						InventoryListener->SlotChanged(inSlot);
						InventoryListener->SlotChanged(slot);
					}
					return;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Unable to split slot"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Split slot index %d our of range"), Index);
	}
}



void
UInventory::DropItemsFromSlot(FItemSlot &slot, int inCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory: Dropped %d item %s"), inCount, *slot.Item.Name);
	auto count = inCount;
	auto& item = slot.Item;
	if (inCount >= slot.Count)
	{
		count = slot.Count;
		if (slot.ShouldRemoveWhenEmpty)
		{
			if (InventoryListener)
				InventoryListener->SlotRemoved(slot);
			Slots.RemoveSingle(slot);
		}
		else
		{
			slot.Count = 0;
		}
	}
	else
	{
		slot.Count -= inCount;
	}

	for (int i = 0; i < count; i++)
	{
		DropItem(item);
	}

	if (slot.Count == 0)
		slot.Item = FItemStruct();

	if (InventoryListener)
		InventoryListener->SlotChanged(slot);
}

void
UInventory::DropItem(const FItemStruct& inItem)
{
	if (Player)
		Player->CreateDroppedItem(inItem);
}
