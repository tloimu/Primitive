// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "PrimitiveCharacter.h"

// ===========================================
// class FItemSlot
// ===========================================

bool
FItemSlot::operator== (const FItemSlot rhs) const
{
	if (Item.Id == rhs.Item.Id && Item.Quality == rhs.Item.Quality)
	{
		return true;
	}
	else return false;
}

bool
FItemSlot::MergeTo(FItemSlot& ToSlot, int inCount)
{
	if (CanMergeTo(ToSlot))
	{
		auto n = FMath::Min(Item.MaxStackSize - ToSlot.Count, inCount);
		if (n > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Merge from slot %d to %d items %d"), Index, ToSlot.Index, inCount);
			ToSlot.Item = Item;
			ChangeCountBy(-n).NotifyChange();
			ToSlot.ChangeCountBy(n).NotifyChange();
			return true;
		}
	}
	return false;
}

bool
FItemSlot::CanMergeTo(FItemSlot& ToSlot) const
{
	if (ToSlot.Count == 0)
	{
		if (ToSlot.CanOnlyWearIn.IsEmpty())
			return true;
		/*
		for (auto bp : Item.CanWearIn)
		{
			UE_LOG(LogTemp, Warning, TEXT("FromSlot CanWear in %d"), bp);
		}
		for (auto bp : ToSlot.CanOnlyWearIn)
		{
			UE_LOG(LogTemp, Warning, TEXT("ToSlot CanWear in %d"), bp);
		}
		*/

		if (!ToSlot.CanOnlyWearIn.Intersect(Item.CanWearIn).IsEmpty())
			return true;
	}
	else
	{
		if (ToSlot.Item == Item)
		{
			if (ToSlot.Count < ToSlot.Item.MaxStackSize)
				return true;
		}
	}

	return false;
}

FItemSlot&
FItemSlot::SetCount(int inCount)
{
	Count = inCount;
	check(Count >= 0);
	if (Count == 0)
		Item = FItemStruct();
	return *this;
}

FItemSlot&
FItemSlot::ChangeCountBy(int inCount)
{
	Count += inCount;
	check(Count >= 0);
	if (Count == 0)
		Item = FItemStruct();
	return *this;
}

void
FItemSlot::NotifyChange() const
{
	if (Inventory && Inventory->InventoryListener)
		Inventory->InventoryListener->SlotChanged(*this);
}


// ===========================================
// class UInventory
// ===========================================

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

int
UInventory::CountItemsOf(const FString& inId) const
{
	int count = 0;
	for (auto& slot : Slots)
	{
		if (slot.Count > 0 && slot.Item.Id == inId)
			count = count + slot.Count;
	}
	return count;
}

bool
UInventory::CanMergeWith(FItemSlot& ToSlot, FItemSlot& FromSlot) const
{
	return FromSlot.CanMergeTo(ToSlot);
}

void
UInventory::MergeWith(FItemSlot& ToSlot, FItemSlot& FromSlot, int inCount)
{
	FromSlot.MergeTo(ToSlot, inCount);
}


bool
UInventory::AddItem(const FItemStruct& item, int count)
{
	FString className = item.ItemClass.Get() ? item.ItemClass.Get()->GetName() : FString("<unknown class>");
	UE_LOG(LogTemp, Warning, TEXT("Adding %d item %s to slot (slots=%d) icon=%s"), count, *className, Slots.Num(), *item.Icon.GetAssetName());

	// First, try to fill slots that already has the same stuff and has room left
	for (int i = 0; i < Slots.Num(); i++)
	{
		auto& slot = Slots[i];
		if (!slot.Inventory)
			UE_LOG(LogTemp, Error, TEXT("NULL inventory on slot %d"), i);
		int fitsInSlot = FMath::Min(item.MaxStackSize - slot.Count, count);
		if (slot.Item == item && fitsInSlot > 0)
		{
			slot.ChangeCountBy(fitsInSlot).NotifyChange();
			count -= fitsInSlot;
			if (count == 0)
				return true;
		}
	}

	// Then, start filling empty slots with more items if needed
	for (int i = 0; i < Slots.Num(); i++)
	{
		auto& slot = Slots[i];
		int fitsInSlot = FMath::Min(item.MaxStackSize - slot.Count, count);
		if (slot.Count == 0 && fitsInSlot > 0)
		{
			slot.Item = item;
			slot.ChangeCountBy(fitsInSlot).NotifyChange();
			count -= fitsInSlot;
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
			slot.ChangeCountBy(-1).NotifyChange();
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
					slot.ChangeCountBy(n).NotifyChange();
					inSlot.ChangeCountBy(-n).NotifyChange();
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
	auto count = FMath::Min(inCount, slot.Count);
	for (int i = 0; i < count; i++)
	{
		DropItem(slot.Item);
	}

	slot.ChangeCountBy(-count).NotifyChange();
}

void
UInventory::DropItem(const FItemStruct& inItem)
{
	if (Player)
		Player->DropItem(inItem);
}
