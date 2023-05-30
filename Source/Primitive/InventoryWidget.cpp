#include "InventoryWidget.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
	bIsFocusable = true;
	Capacity = 30;
}

bool UInventoryWidget::AddItem(AInteractableActor* item)
{
	if (Items.Num() >= Capacity)
		return false;

	Items.Add(item);
	ItemAdded(item);
	return true;
}

bool UInventoryWidget::RemoveItem(AInteractableActor* item)
{
	if (Items.RemoveSingle(item) == 1)
	{
		ItemRemoved(item);
		return true;
	}
	else
		return false;
}

const TArray<AInteractableActor*> UInventoryWidget::GetItems() const
{
	return Items;
}

void UInventoryWidget::ItemAdded_Implementation(AInteractableActor* item)
{
	auto name = item->GetActorNameOrLabel();
	UE_LOG(LogTemp, Warning, TEXT("Added Item to Inventory %s"), *name);
}

void UInventoryWidget::ItemRemoved_Implementation(AInteractableActor* item)
{
	auto name = item->GetActorNameOrLabel();
	UE_LOG(LogTemp, Warning, TEXT("Removed Item from Inventory %s"), *name);
}
