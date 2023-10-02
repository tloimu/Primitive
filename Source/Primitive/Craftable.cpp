// Fill out your copyright notice in the Description page of Project Settings.


#include "Craftable.h"
#include "Kismet/GameplayStatics.h"
#include "CrafterSlot.h"
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
	UE_LOG(LogTemp, Warning, TEXT("Crafter: Check ability to start crafting %s"), *inRecipie.Id);

	if (!Works.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafter: Cannot start crafting as there is already a work in progress"), *inRecipie.Id);
		return false;
	}

	// Check nearby inventories for ingredients and start the craft if possible
	auto missingIngredients = inRecipie.Ingredients;
	TArray<TPair<FItemSlot*, int>> usedSlots;
	for (auto &ing : missingIngredients)
	{
		if (ing.ItemCount > 0)
		{
			for (auto inv : inIngredientInventories)
			{
				for (auto& slot : inv->Slots)
				{
					if (slot.Count > 0 && slot.Item.Id == ing.ItemId && slot.Item.Quality >= ing.MinimumQuality)
					{
						int n = slot.Count;
						if (slot.Count > ing.ItemCount)
							n = ing.ItemCount;
						ing.ItemCount -= n;
						usedSlots.Add({ &slot, n });
						if (ing.ItemCount == 0)
							break;
					}
				}
				if (ing.ItemCount == 0)
					break;
			}
		}
	}

	for (auto& ing : missingIngredients)
	{
		if (ing.ItemCount > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Crafter: Cannot start crafting, missing at least %d %s"), ing.ItemCount, *ing.ItemId);
			return false;
		}
	}

	for (auto &slot : usedSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafter: Used %d %s from slot %d/%p"), slot.Value, *slot.Key->Item.Id, slot.Key->Index, slot.Key->Inventory);
		slot.Key->Count -= slot.Value;
		if (slot.Key->Count == 0)
			slot.Key->Item = FItemStruct();
		if (slot.Key->Inventory && slot.Key->Inventory->InventoryListener)
		{
			slot.Key->Inventory->InventoryListener->SlotChanged(*slot.Key);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Crafter: Start crafting %s"), *inRecipie.Id);
	FCraftingWork work;
	work.Id = NextWorkId++;
	work.Recipie = inRecipie;
	work.GameTimeProgressLeft = inRecipie.CraftingSeconds;
	Works.Add(work);

	if (Inventory && Inventory->Player)
	{
		auto player = Cast<APrimitiveCharacter>(Inventory->Player);
		if (player && player->HandCraftingSound)
		{
			UGameplayStatics::PlaySound2D(player->GetWorld(), player->HandCraftingSound);
		}
	}

	return true;
}

void
UCrafter::CheckCrafting(float DeltaGameTimeSecs)
{
	bool hasCompletion = false;
	for (int i = Works.Num() - 1; i >= 0; i--)
	{
		auto& w = Works[i];
		if (w.GameTimeProgressLeft <= DeltaGameTimeSecs)
		{
			hasCompletion = true;
			w.GameTimeProgressLeft = 0.0f;
			CompleteCrafting(w);
			Works.RemoveAt(i);
		}
		else
		{
			w.GameTimeProgressLeft -= DeltaGameTimeSecs;
			if (w.Slot)
				w.Slot->SetProgress(w.GameTimeProgressLeft / w.Recipie.CraftingSeconds);
		}
	}
}

void
UCrafter::CompleteCrafting(FCraftingWork& Work)
{
	UE_LOG(LogTemp, Warning, TEXT("Crafter: Completed crafting %s"), *Work.Recipie.Id);
	if (Inventory)
	{
		auto item = Inventory->FindItem(Work.Recipie.CraftedItemId);
		if (item)
		{
			if (!Inventory->AddItem(*item, Work.Recipie.CraftedItemCount))
			{
				for (int i = 0; i < Work.Recipie.CraftedItemCount; i++)
					Inventory->DropItem(*item);
			}
		}
	}
}
