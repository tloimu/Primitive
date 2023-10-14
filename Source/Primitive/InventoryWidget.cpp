#include "InventoryWidget.h"
#include "InventorySlotDragOperation.h"
#include "InteractableActor.h"
#include "Inventory.h"
#include "CrafterSlot.h"
#include "Blueprint/WidgetTree.h"
#include "PrimitiveCharacter.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer)
{
	bIsFocusable = true;
	InventorySlotClass = StaticClass();
}

void
UInventoryWidget::Setup(UInventory* inInventory, UInventory* inEquippedItems)
{
	Inventory = inInventory;
	Inventory->InventoryListener = this;
	EquippedItems = inEquippedItems;
	EquippedItems->InventoryListener = this;

	SetupEquippedSlots();
}


void FindAllInventorySlots(UUserWidget* from, TArray<UInventorySlot*>& outSlots)
{
	TArray<UWidget*> children;
	from->WidgetTree->GetAllWidgets(children);

	for (auto& w : children)
	{
		auto slot = Cast<UInventorySlot>(w);
		if (slot)
			outSlots.Add(slot);
		auto uw = Cast<UUserWidget>(w);
		if (uw)
			FindAllInventorySlots(uw, outSlots);
	}
}

void
UInventoryWidget::SetupEquippedSlots()
{
	if (EquippedItems)
	{
		TArray<UInventorySlot*> eqSlots;
		FindAllInventorySlots(this, eqSlots);

		int i = 0;
		for (auto es : eqSlots)
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipmentSlot [%d] [%p]"), i, es);
			if (es)
			{
				FItemSlot slot;
				slot.Index = i;
				slot.Inventory = EquippedItems;
				if (es->EquippedIn != BodyPart::None)
					slot.CanOnlyWearIn.Add(es->EquippedIn);
				es->SlotIndex = i;
				es->Inventory = EquippedItems;
				EquippedItems->Slots.Add(slot);
				EquipmentSlots.Add(es);
				UE_LOG(LogTemp, Warning, TEXT("EquipmentSlot [%d] in [%d]: Set"), i, es->EquippedIn);
				slot.NotifyChange();
			}
			i++;
		}
	}
}

void
UInventoryWidget::SetContainerInventory(UInventory* inContainerInventory)
{
	ContainerInventory = inContainerInventory;
	if (ContainerInventory)
	{
		// ???? TODO: Set container inventory segment visible add the container's item slots and items into them
	}
	else
	{
		// ???? TODO: Clear and hide the container inventory segment
	}
}


void
UInventoryWidget::SlotChanged(const FItemSlot& inSlot)
{
	UInventorySlot* SlotUI = nullptr;
	if (inSlot.Inventory == Inventory)
	{
		if (inSlot.Index < 0 || inSlot.Index >= Slots.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Attempt to put item %s to inventory slot %d"), *inSlot.Item.Id, inSlot.Index);
			return;
		}
		while (inSlot.Index >= Slots.Num())
		{
			AddNewInventorySlot();
		}
		SlotUI = Slots[inSlot.Index];
	}
	else if (inSlot.Inventory == EquippedItems)
	{
		if (inSlot.Index < 0 || inSlot.Index >= EquipmentSlots.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Attempt to put item %s to equipment slot %d"), *inSlot.Item.Id, inSlot.Index);
			return;
		}

		SlotUI = EquipmentSlots[inSlot.Index];
	}
	else if (inSlot.Inventory == ContainerInventory)
	{
		if (inSlot.Index < 0 || inSlot.Index >= ContainerSlots.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Attempt to put item %s to container slot %d"), *inSlot.Item.Id, inSlot.Index);
			return;
		}
		SlotUI = ContainerSlots[inSlot.Index];
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Setting to unknown inventory %p to slot %d"), inSlot.Inventory, inSlot.Index);
	}

	if (inSlot.Count > 0)
		inSlot.Item.Icon.LoadSynchronous();
	if (SlotUI)
		SlotUI->SlotSet(inSlot);
}

void
UInventoryWidget::MaxSlotsChanged(int inMaxSlots)
{
	while (inMaxSlots > Slots.Num())
	{
		AddNewInventorySlot();
	}

	while (inMaxSlots < Slots.Num())
	{
		RemoveSlot(Slots.Num() - 1);
	}
}

void
UInventoryWidget::SlotRemoved(const FItemSlot& inSlot)
{
	RemoveSlot(inSlot.Index);
}

UInventorySlot*
UInventoryWidget::AddNewInventorySlot()
{
	if (Inventory)
	{
		auto slot = MakeNewSlot(Inventory, Slots.Num());
		Slots.Add(slot);
		InventorySlotAdded(slot);
		return slot;
	}
	return nullptr;
}

UInventorySlot*
UInventoryWidget::SetNewEquipmentSlot()
{
	if (EquippedItems)
	{
		auto slot = MakeNewSlot(EquippedItems, EquipmentSlots.Num());
		EquipmentSlots.Add(slot);
		return slot;
	}
	return nullptr;
}

UInventorySlot*
UInventoryWidget::AddNewContainerSlot()
{
	if (ContainerInventory)
	{
		auto slot = MakeNewSlot(ContainerInventory, ContainerSlots.Num());
		ContainerSlots.Add(slot);
		ContainerSlotAdded(slot);
		return slot;
	}
	return nullptr;
}

UCrafterSlot*
UInventoryWidget::AddNewCrafterSlot()
{
	FString name = FString::Format(TEXT("CrafterSlot/{0}"), { CrafterSlots.Num() });
	auto slot = CreateWidget<UCrafterSlot>(this, CrafterSlotClass, *name);
	if (slot)
	{
		slot->SlotIndex = CrafterSlots.Num();
		CrafterSlots.Add(slot);
	}
	return slot;
}

void
UInventoryWidget::CloseCrafter()
{
	for (auto slot : CrafterSlots)
	{
		slot->SlotRemoved();
	}
	CrafterSlots.Empty();
}

UInventorySlot*
UInventoryWidget::MakeNewSlot(UInventory* inInventory, int SlotIndex)
{
	FString name = FString::Format(TEXT("InventorySlot/{0}/{1}"), { SlotIndex, inInventory->GetName() });
	auto slot = CreateWidget<UInventorySlot>(this, InventorySlotClass, *name);
	if (slot)
	{
		FItemSlot empty;
		empty.Index = SlotIndex;
		empty.Inventory = inInventory;
		slot->Inventory = empty.Inventory;
		slot->SlotIndex = empty.Index;
		slot->SlotSet(empty);
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
}

void
UInventoryWidget::ContainerOpened_Implementation(const FString& inName)
{
	UE_LOG(LogTemp, Warning, TEXT("Container %s Opened"), *inName);
}

void
UInventoryWidget::ContainerClosed_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Container Opened"));
}

void
UInventoryWidget::ContainerSlotAdded_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Added slot to Container"));
}

void
UInventoryWidget::ContainerSlotRemoved_Implementation(UInventorySlot* inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Removed slot from Container"));
}

void
UInventoryWidget::CraftingSlotAdded_Implementation(UCrafterSlot* inSlot, const FItemStruct &inItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Added crafting recipie slot"));
}

void
UInventoryWidget::CrafterClosed_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Removed crafting recipie slot"));
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
	if (Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation))
		return true;

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
	if (Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation))
		return true;

	UE_LOG(LogTemp, Warning, TEXT("Inventory: OnDrop"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && dragged->Inventory)
	{
		auto n = dragged->Inventory->GetSlotAt(dragged->SlotIndex).Count;
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

void
UInventoryWidget::SetContainer(AInteractableActor *inContainer)
{
	if (inContainer && inContainer->Inventory)
	{
		auto inv = inContainer->Inventory;
		if (inv != ContainerInventory)
		{
			inv->InventoryListener = this;
			for (auto& slot : ContainerSlots)
				ContainerSlotRemoved(slot);
			ContainerSlots.Empty();
			ContainerInventory = inv;
			for (auto& slot : inv->Slots)
			{
				auto slotUi = MakeNewSlot(inv, ContainerSlots.Num());
				ContainerSlots.Add(slotUi);
				slotUi->SlotSet(slot);
				ContainerSlotAdded(slotUi);
			}
			ContainerOpened(inContainer->Item.Name);
		}
	}
	else
	{
		if (ContainerInventory)
		{
			ContainerInventory->InventoryListener = nullptr;
			for (auto& slot : ContainerSlots)
				ContainerSlotRemoved(slot);
			ContainerSlots.Empty();
			ContainerInventory = nullptr;
			ContainerClosed();
		}
	}
}
