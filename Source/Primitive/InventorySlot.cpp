#include "InventorySlot.h"
#include "InventoryWidget.h"
#include "InventorySlotDragOperation.h"

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
	if (ItemCount == 0 && Inventory)
	{
		Item.Icon = Inventory->EmptySlotIcon;
		Item.Name = TEXT("Empty");
	}

	ItemSet(Item, ItemCount);
}

void
UInventorySlot::Clear()
{
	ItemCount = -1;
	Cleared();
}

void
UInventorySlot::SetEmpty()
{
	if (Inventory)
	{
		Item.Icon = Inventory->EmptySlotIcon;
	}
	Item.Name = TEXT("Empty");
	Item.MaxStackSize = 0;
	ItemCount = 0;
	ItemSet(Item, ItemCount);
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

void
UInventorySlot::SetHighlight_Implementation(bool DoHighlight)
{
	UE_LOG(LogTemp, Warning, TEXT("Highlight %d"), DoHighlight);
}


// Drag'n'Drop

void UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("Drag enter"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (CanMergeWith(dragged))
		SetHighlight(true);
}

void UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("Drag leave"));
	SetHighlight(false);
}

bool UInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("Drag over"));
	auto oper = Cast<UInventorySlot>(InOperation->Payload);
	if (oper != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from %s to %s"), *oper->GetItem().Name,*Item.Name);
	}
	return true;
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDrop"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (CanMergeWith(dragged))
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropped item %s"), *dragged->GetItem().Name);
		MoveItemsHereFromSlot(dragged);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropped incompatible item"));
		return true;
	}
	return true;
}

// Splitting and merging

FReply
UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (reply.IsEventHandled())
		return reply;

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		Split();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


bool UInventorySlot::CanMergeWith(UInventorySlot* Other) const
{
	return (IsEmpty() || (Other && Other != this && Other->Item.Id == Item.Id && ItemCount < Item.MaxStackSize));
}

void UInventorySlot::MoveItemsHereFromSlot(UInventorySlot* Other)
{
	if (IsEmpty())
	{
		Item = Other->Item;
	}

	auto n = Other->ItemCount;
	if (ItemCount + n > Item.MaxStackSize)
		n = Item.MaxStackSize - ItemCount + n;
	ItemCount += n;
	Other->ItemCount -= n;
	if (Other->ItemCount < 1)
		Other->SetItemCount(0);
	else
		Other->SetItemCount(Other->ItemCount);
	SetItemCount(ItemCount);
}

void UInventorySlot::Split()
{
	UE_LOG(LogTemp, Warning, TEXT("Split slot"));
	if (ItemCount > 1)
	{
		auto n = ItemCount / 2;
		if (Inventory)
		{
			auto slot = Inventory->AddToNewSlot(Item, n);
			if (slot)
			{
				SetItemCount(ItemCount - slot->ItemCount);
			}
		}
	}
}