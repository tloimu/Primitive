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
