#include "InventoryWidget.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
	Capacity = 30;
}

bool UInventoryWidget::AddItem(AInteractableActor* item)
{
	if (Items.Num() >= Capacity)
		return false;

	Items.Add(item);
	return true;
}

bool UInventoryWidget::RemoveItem(AInteractableActor* item)
{
	return (Items.RemoveSingle(item) == 1);
}

const TArray<AInteractableActor*> UInventoryWidget::GetItems() const
{
	return Items;
}
