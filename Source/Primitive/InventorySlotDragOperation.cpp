// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"
#include "InventorySlotDragOperation.h"

void
UDraggedInventorySlot::SetInventorySlot(UInventorySlot* inSlot)
{
	if (inSlot)
	{
		FromSlot = inSlot;
		auto& slot = inSlot->Inventory->GetSlotAt(inSlot->SlotIndex);

		if (slot.Count > 0)
			SizeText = FString::Format(TEXT("{0}"), { slot.Count });

		Icon = slot.Item.Icon;

		UpdateDragged(SizeText, Icon);
	}
}

void
UDraggedInventorySlot::UpdateDragged_Implementation(const FString &txt, const TSoftObjectPtr<UTexture> &icn)
{
//	UE_LOG(LogTemp, Warning, TEXT("Update dragged inventory slot: %s"), *SizeText);
}
