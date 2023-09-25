#include "InventoryWidget.h"
#include "InventorySlotDragOperation.h"
#include "InteractableActor.h"
#include "Inventory.h"
#include "PrimitiveCharacter.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
	bIsFocusable = true;
	// MaxSlots = 30;
	InventorySlotClass = StaticClass();
}

void
UInventoryWidget::SlotChanged(int Index, const FItemSlot& inSlot)
{
	while (Index >= Slots.Num())
	{
		AddNewSlot();
	}

	auto SlotUI = Slots[Index];
	if (inSlot.Count > 0)
		inSlot.Item.Icon.LoadSynchronous();
	SlotUI->SlotSet(inSlot);
}

void
UInventoryWidget::MaxSlotsChanged(int inMaxSlots)
{
	while (inMaxSlots > Slots.Num())
	{
		AddNewSlot();
	}

	while (inMaxSlots < Slots.Num())
	{
		RemoveSlot(Slots.Num() - 1);
	}
}

void
UInventoryWidget::SlotRemoved(int Index)
{
	RemoveSlot(Index);
}


UInventorySlot*
UInventoryWidget::AddNewSlot()
{
	auto slot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
	if (slot)
	{
		FItemSlot empty;
		empty.Index = Slots.Num();
		slot->Inventory = Inventory;
		slot->SlotIndex = empty.Index;
		slot->SlotSet(empty);
		Slots.Add(slot);
		InventorySlotAdded(slot);
		return slot;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create inventory slot due missing InventorySlotClass"));
		return nullptr;
	}
}

void
UInventoryWidget::RemoveSlot(int Index)
{
	auto slot = Slots[Index];
	slot->Inventory = nullptr;
	slot->RemoveFromParent();
	Slots.RemoveAt(Index);
	InventorySlotsChanged();
}

void
UInventoryWidget::InventorySlotAdded_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Added slot to Inventory"));
}

void
UInventoryWidget::InventorySlotRemoved_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Removed slot from Inventory"));
}

void
UInventoryWidget::InventorySlotsChanged_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SlotsChanged count %d"), Slots.Num());
/*	for (auto i = Slots.Num(); i < MaxSlots; i++)
	{
		FItemStruct empty;
		empty.Icon = EmptySlotIcon;
		AddToNewSlot(empty, 0);
	}
	// ???? TODO: Remove extra slots
	*/
}

// Drag'n'Drop

void
UInventoryWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag enter"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from slot %d"), dragged->SlotIndex);
	}
}

void
UInventoryWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag leave"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drag over from slot %d"), dragged->SlotIndex);
	}
}

bool
UInventoryWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	// UE_LOG(LogTemp, Warning, TEXT("Inventory: Drag over"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Drag over from slot %d"), dragged->SlotIndex);
	}
	return true;
}

bool
UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Inventory: OnDrop"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && Inventory)
	{
		auto n = Inventory->GetSlotAt(dragged->SlotIndex).Count;
		if (InDragDropEvent.GetModifierKeys().IsShiftDown())
			n = 1;
		DropItemsFromSlot(dragged, n);
	}
	return true;
}

void
UInventoryWidget::DropItemsFromSlot(UInventorySlot* inSlot, int inCount)
{
	if (Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget: Drop %d from slot %d"), inCount, inSlot->SlotIndex);
		auto& slot = Inventory->GetSlotAt(inSlot->SlotIndex);
		Inventory->DropItemsFromSlot(slot, inCount);
	}
}
