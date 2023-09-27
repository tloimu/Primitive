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
	UE_LOG(LogTemp, Warning, TEXT("Set %d (%s) to slot of %d now"), inSlot.Index, *inSlot.Item.Name, inSlot.Count);
	SlotIndex = inSlot.Index;
}

void
UInventorySlot::SlotRemoved_Implementation(const FItemSlot& inSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot %d (%s) removed"), inSlot.Index, *inSlot.Item.Name);
}

void
UInventorySlot::SetHighlight_Implementation(bool DoHighlight)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot %d highlight %d"), SlotIndex, DoHighlight);
}

// Drag'n'Drop

void
UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UE_LOG(LogTemp, Warning, TEXT("Drag detected slot %d"), SlotIndex);

	if (!IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start dragging slot %d..."), SlotIndex);
		auto oper = Cast<UInventorySlotDragOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventorySlotDragOperation::StaticClass()));
		if (oper)
		{
			auto dragged = CreateWidget<UDraggedInventorySlot>(this, DraggedInventorySlotWidgetClass);
			if (dragged)
			{
				UE_LOG(LogTemp, Warning, TEXT("Dragging slot %d %s"), SlotIndex, *DraggedInventorySlotWidgetClass.Get()->GetName());
				dragged->SetInventorySlot(this);
				oper->InventorySlot = dragged;
				oper->Payload = this;

				oper->DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
				oper->DefaultDragVisual = this;
				oper->Pivot = EDragPivot::MouseDown;

				OutOperation = oper;
			}
		}
	}
}

void
UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("Drag enter slot %d"), SlotIndex);
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && Inventory)
	{
		auto& fromSlot = Inventory->GetSlotAt(dragged->SlotIndex);
		auto& toSlot = Inventory->GetSlotAt(SlotIndex);
		if (Inventory->CanMergeWith(toSlot, fromSlot))
			SetHighlight(true);
	}
}

void UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("Drag leave slot %d"), SlotIndex);
	SetHighlight(false);
}

bool UInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
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
	UE_LOG(LogTemp, Warning, TEXT("OnDrop"));
	auto dragged = Cast<UInventorySlot>(InOperation->Payload);
	if (dragged && Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropped item"));
		auto& fromSlot = Inventory->GetSlotAt(dragged->SlotIndex);
		auto& toSlot = Inventory->GetSlotAt(SlotIndex);
		auto n = fromSlot.Count;
		if (InDragDropEvent.GetModifierKeys().IsShiftDown())
			n = 1;
		if (Inventory->CanMergeWith(toSlot, fromSlot))
			Inventory->MergeWith(toSlot, fromSlot, n);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropped incompatible item"));
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
		Inventory->SplitSlot(SlotIndex);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
UInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Mouse enter slot %d"), SlotIndex);
	SetHighlight(true);
	if (Inventory)
	{
		Inventory->CurrentSelectedSlotIndex = SlotIndex;
	}
}

void
UInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Mouse leave slot %d"), SlotIndex);
	SetHighlight(false);
	if (Inventory)
	{
		if (Inventory->CurrentSelectedSlotIndex == SlotIndex)
			Inventory->CurrentSelectedSlotIndex = -1;
	}
}
