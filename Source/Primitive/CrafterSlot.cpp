// Fill out your copyright notice in the Description page of Project Settings.


#include "CrafterSlot.h"
#include "Components/AudioComponent.h"
#include "PrimitiveCharacter.h"


UCrafterSlot::UCrafterSlot(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
}

void
UCrafterSlot::SlotSet_Implementation(const FCraftRecipie& inSlot, const FItemStruct& inItem)
{
}

void
UCrafterSlot::SlotRemoved_Implementation()
{
}

void
UCrafterSlot::SetHighlight_Implementation(bool DoHighlight)
{
}

void
UCrafterSlot::SetProgress_Implementation(float Progress)
{
}

void
UCrafterSlot::SetSlot(const FCraftRecipie& inSlot, const FItemStruct& inItem)
{
	Recipie = inSlot;
	Item = inItem;
	SlotSet(inSlot, inItem);
}


FReply
UCrafterSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter Slot [%d]: Mouse down"), SlotIndex);
	auto reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (reply.IsEventHandled())
		return reply;

	if (Inventory && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafter Slot [%d]: Start crafting"), SlotIndex);
		if (Crafter)
		{
			if (Crafter->StartCrafting(Recipie, { Inventory }))
			{
			}
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
UCrafterSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter Slot [%d]: Mouse enter"), SlotIndex);
	SetHighlight(true);
}

void
UCrafterSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter Slot [%d]: Mouse leave"), SlotIndex);
	SetHighlight(false);
}

