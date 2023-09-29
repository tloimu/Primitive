#include "InventorySlot.h"
#include "InventoryWidget.h"
#include "PrimitiveCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InventorySlotDragOperation.h"

UInventorySlot::UInventorySlot(const FObjectInitializer& ObjectInitializer): UUserWidget(ObjectInitializer), Inventory(nullptr), SlotIndex(0)
{
}

bool
UInventorySlot::IsEmpty()
{
	if (Inventory)
	{
		return Inventory->GetSlotAt(SlotIndex).Count == 0;
	}
	return true;
}

int
UInventorySlot::GetItemCount()
{
	if (Inventory)
	{
		return Inventory->GetSlotAt(SlotIndex).Count;
	}
	return 0;
}

UTexture*
UInventorySlot::GetIcon()
{
	if (Inventory)
	{
		return Inventory->GetSlotAt(SlotIndex).Item.Icon.Get();
	}
	return 0;
}

void
UInventorySlot::SlotSet_Implementation(const FItemSlot& inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Set %s to slot of %d now"), inSlot.Index, *inSlot.Item.Name, inSlot.Count);
	SlotIndex = inSlot.Index;
}

void
UInventorySlot::SlotRemoved_Implementation(const FItemSlot& inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Removed %s"), inSlot.Index, *inSlot.Item.Name);
}

void
UInventorySlot::SetHighlight_Implementation(bool DoHighlight)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Highlight %d"), SlotIndex, DoHighlight);
}

// Drag'n'Drop

void
UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Drag detected"), SlotIndex);

	if (!IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Start dragging..."), SlotIndex);
		auto oper = Cast<UInventorySlotDragOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventorySlotDragOperation::StaticClass()));
		if (oper)
		{
			auto dragged = CreateWidget<UDraggedInventorySlot>(this, DraggedInventorySlotWidgetClass);
			if (dragged)
			{
				UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Dragging %s"), SlotIndex, *DraggedInventorySlotWidgetClass.Get()->GetName());
				dragged->SetInventorySlot(this);
				oper->InventorySlot = dragged;
				oper->Payload = this;

				oper->DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
				oper->DefaultDragVisual = dragged;
				oper->Pivot = EDragPivot::MouseDown;

				OutOperation = oper;
			}
		}
	}
}

void
UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Drag enter"), SlotIndex);
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && Inventory)
	{
		auto& fromSlot = dragged->Inventory->GetSlotAt(dragged->SlotIndex);
		auto& toSlot = Inventory->GetSlotAt(SlotIndex);
		if (Inventory->CanMergeWith(toSlot, fromSlot))
			SetHighlight(true);
	}
}

void
UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Drag leave"), SlotIndex);
	SetHighlight(false);
}

bool
UInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation))
		return true;

	// UE_LOG(LogTemp, Warning, TEXT("Drag over slot %d"), SlotIndex);
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Drag over from slot index %d to %d"), dragged->SlotIndex, SlotIndex);
	}
	return true;
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation))
		return true;

	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: OnDrop"), SlotIndex);
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && Inventory)
	{
		auto& fromSlot = dragged->Inventory->GetSlotAt(dragged->SlotIndex);
		auto& toSlot = Inventory->GetSlotAt(SlotIndex);
		auto n = fromSlot.Count;
		UE_LOG(LogTemp, Warning, TEXT("Slot [%d/%p]: Dropped %d item %s from slot [%d/%p]"), SlotIndex, Inventory, n, *fromSlot.Item.Id, fromSlot.Inventory);
		if (InDragDropEvent.GetModifierKeys().IsShiftDown())
			n = 1;
		if (Inventory->CanMergeWith(toSlot, fromSlot))
			Inventory->MergeWith(toSlot, fromSlot, n);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Dropped incompatible item"), SlotIndex);
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

	if (Inventory && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		Inventory->SplitSlot(SlotIndex);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
UInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Mouse enter"), SlotIndex);
	SetHighlight(true);
	if (Inventory)
	{
		Inventory->CurrentSelectedSlotIndex = SlotIndex;
		auto &slot = Inventory->GetSlotAt(SlotIndex);
		UE_LOG(LogTemp, Warning, TEXT("Slot [%d/%p]: %d %s"), slot.Index, slot.Inventory, slot.Count, *slot.Item.Id);
	}
}

void
UInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot [%d]: Mouse leave"), SlotIndex);
	SetHighlight(false);
	if (Inventory)
	{
		if (Inventory->CurrentSelectedSlotIndex == SlotIndex)
			Inventory->CurrentSelectedSlotIndex = -1;
	}
}
