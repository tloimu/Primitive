#include "ItemContainer.h"
/*
AItemContainer::AItemContainer()
{
	Capacity = 1;
}

bool AItemContainer::AddItem(UItemSpecs* item)
{
	if (Items.Num() >= Capacity)
		return false;

	Items.Add(item);
	ItemAdded(item);
	return true;
}

bool AItemContainer::RemoveItem(UItemSpecs* item)
{
	if (Items.RemoveSingle(item) == 1)
	{
		ItemRemoved(item);
		return true;
	}

	return false;
}

const TArray<UItemSpecs*> AItemContainer::GetItems() const
{
	return Items;
}


void AItemContainer::ItemAdded_Implementation(UItemSpecs* item)
{
	auto name = item->GetName();
	UE_LOG(LogTemp, Warning, TEXT("Added Item to Container %s"), *name);
}

void AItemContainer::ItemRemoved_Implementation(UItemSpecs* item)
{
	auto name = item->GetName();
	UE_LOG(LogTemp, Warning, TEXT("Removed Item from Container %s"), *name);
}
*/