#include "InteractableActor.h"

void AInteractableActor::Interact_Implementation() {}
void AInteractableActor::Hit_Implementation() {}
void AInteractableActor::Consume_Implementation() {}

bool
AInteractableActor::IsEmpty() const
{
	if (Inventory)
		return Inventory->IsEmpty();
	else
		return true;
}

bool
AInteractableActor::CanBePicked() const
{
	if (Inventory)
	{
		if (!Item.CanWearIn.IsEmpty())
			return true;
		if (!Inventory->IsEmpty())
			return false;
	}
	return true;
}


void
AInteractableActor::AddOnItem(AInteractableActor& inSupport)
{
	UE_LOG(LogTemp, Warning, TEXT("Add item %s on top of %s"), *GetName(), *inSupport.GetName());
	SupportedByItem = &inSupport;
	inSupport.SupportsItems.Add(this);
}

void
AInteractableActor::DestroyItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Remove item %s"), *GetName());

	if (SupportedByItem)
	{
		SupportedByItem->SupportsItems.RemoveSingle(this);
		SupportedByItem = nullptr;
	}

	auto supported = SupportsItems;
	for (auto item : supported)
	{
		item->DestroyItem();
	}

	Destroy();
}
