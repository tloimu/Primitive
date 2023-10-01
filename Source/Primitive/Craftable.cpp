// Fill out your copyright notice in the Description page of Project Settings.


#include "Craftable.h"
#include "PrimitiveCharacter.h"

const FString UCrafter::HandCraftingStationItemId = "Hand";

UCrafter::UCrafter(): UObject()
{
}

UCrafter::UCrafter(const FObjectInitializer& ObjectInitializer): UObject(ObjectInitializer)
{
}

void
UCrafter::CraftingStarted_Implementation(const FCraftingWork &inWork)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter: Started work %d"), inWork.Id);
}

void
UCrafter::CraftingProgressing_Implementation(const FCraftingWork &inWork)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter: Progressing work %d"), inWork.Id);
}

void
UCrafter::CraftingCompleted_Implementation(const FCraftingWork &inWork)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter: Completed work %d"), inWork.Id);
}

void
UCrafter::Setup()
{
}

bool
UCrafter::CanCraft(const FCraftRecipie& inRecipie, TArray<UInventory*> inIngredientInventories)
{
	// ???? TODO: Check availability of ingredients and capabililty of the crafter
	return true;
}

bool
UCrafter::StartCrafting(const FCraftRecipie& inRecipie, TArray<UInventory*> inIngredientInventories)
{
	FCraftingWork work;
	work.Id = NextWorkId++;
	Works.Add(work);

	// ???? TODO: Check nearby inventories for ingredients and start the craft if possible

	return true;
}

void
UCrafter::CheckCrafting()
{
	// ???? TODO: Check if any work has completed - also, send progress events to UI
}

void
UCrafter::CompleteCrafting(FCraftingWork& inProgress)
{
	// ???? TODO: Create the item and place it into inventory
}


// ---------------------------------------------------------
// class UCrafterSlot widget
// ---------------------------------------------------------


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
		// ???? TODO: Start crafting
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

