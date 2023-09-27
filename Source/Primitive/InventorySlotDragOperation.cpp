// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotDragOperation.h"
#include "InventorySlot.h"

void
UDraggedInventorySlot::SetInventorySlot(UInventorySlot* inSlot)
{
	if (inSlot)
	{
		FromSlot = inSlot;
		auto& slot = inSlot->Inventory->GetSlotAt(inSlot->SlotIndex);

		if (slot.Item.MaxStackSize > 1 && slot.Count > 0)
			SizeText = FString::Format(TEXT("{0}"), { slot.Count });
		else
			SizeText.Empty();

		Icon = slot.Item.Icon;

		UpdateDragged(SizeText, Icon);
	}
}

void
UDraggedInventorySlot::UpdateDragged_Implementation(const FString &txt, const TSoftObjectPtr<UTexture> &icn)
{
//	UE_LOG(LogTemp, Warning, TEXT("Update dragged inventory slot: %s"), *SizeText);
}
