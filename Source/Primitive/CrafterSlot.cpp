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
UCrafterSlot::SetHelpText_Implementation(const FCraftingHelpInfo& inInfo)
{
}

void
UCrafterSlot::HideHelpText_Implementation()
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
				SetSlotHelp();
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
	SetSlotHelp();
}

void
UCrafterSlot::SetSlotHelp()
{
	FCraftingHelpInfo help;
	help.CanCraft = (Crafter && Inventory) ? Crafter->CanCraft(Recipie, { Inventory }) : 0;
	int maxCraftable = 1000;
	auto craftedItem = Inventory ? Inventory->FindItem(Recipie.CraftedItemId) : nullptr;
	if (craftedItem)
		help.Item = *craftedItem;
	for (auto& i : Recipie.Ingredients)
	{
		int count = 0;
		FCraftingHelpIngredient iHelp;
		if (Inventory)
		{
			count = Inventory->CountItemsOf(i.ItemId);
			auto item = Inventory->FindItem(i.ItemId);
			if (item)
			{
				iHelp.Name = item->Name;
			}
		}
		if (count == 0)
			maxCraftable = 0;
		else
			maxCraftable = FMath::Min(maxCraftable, count / i.ItemCount);

		iHelp.Available = count;
		iHelp.Needed = i.ItemCount;
		iHelp.IsSufficient = (iHelp.Available >= iHelp.Needed);
		help.AvailableIngredients.Add(iHelp);
	}
	help.CanCraft = (maxCraftable > 0);
	help.CanCraftCount = maxCraftable;
	SetHelpText(help);
}

void
UCrafterSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter Slot [%d]: Mouse leave"), SlotIndex);
	SetHighlight(false);
	SetHelpText(FCraftingHelpInfo());
	HideHelpText();
}

