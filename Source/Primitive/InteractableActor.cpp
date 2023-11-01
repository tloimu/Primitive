#include "InteractableActor.h"
#include "PrimitiveGameState.h"

bool AInteractableActor::Interact_Implementation() { return false; }
bool AInteractableActor::Hit_Implementation() { return false; }
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

float
AInteractableActor::GetClockSpeed() const
{
	auto gs = GetWorld()->GetGameState<APrimitiveGameState>();
	if (gs)
		return gs->ClockSpeed;
	else
		return 60.0f;
}
