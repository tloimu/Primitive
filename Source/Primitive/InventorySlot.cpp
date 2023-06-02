#include "InventorySlot.h"

UInventorySlot::UInventorySlot(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer), ItemCount(-1)
{
}

void
UInventorySlot::SetItemAndCount(const FItemStruct& inItem, int inCount)
{
	Item = inItem;
	ItemCount = inCount;
	ItemSet(Item, ItemCount);
}

void
UInventorySlot::SetItem(const FItemStruct& inItem)
{
	Item = inItem;
	ItemSet(Item, ItemCount);
}

void
UInventorySlot::SetItemCount(int inCount)
{
	ItemCount = inCount;
	ItemSet(Item, ItemCount);
}

void
UInventorySlot::Clear()
{
	ItemCount = -1;
	Cleared();
}

void
UInventorySlot::ItemSet_Implementation(const FItemStruct& inItem, int inCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Added %s to slot of %d now"), *inItem.Name, inCount);
}

void
UInventorySlot::Cleared_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleared slot of %s"), *Item.Name);
}
