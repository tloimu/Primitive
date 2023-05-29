#include "ItemContainer.h"

AItemContainer::AItemContainer()
{
	Capacity = 1;
}

bool AItemContainer::AddItem(AInteractableActor* item)
{
	if (Items.Num() >= Capacity)
		return false;

	Items.Add(item);
	return true;
}

bool AItemContainer::RemoveItem(AInteractableActor* item)
{
	return (Items.RemoveSingle(item) == 1);
}

const TArray<AInteractableActor*> AItemContainer::GetItems() const
{
	return Items;
}
