#include "CrafterWidget.h"
#include "PrimitiveCharacter.h"
#include "ItemDatabase.h"
#include "Blueprint/WidgetTree.h"
#include "OmaUtil.h"

UCrafterWidget::UCrafterWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	SetIsFocusable(true);
	CrafterSlotClass = StaticClass();
}

void
UCrafterWidget::SetCrafter(UCrafter* inCrafter)
{
	UE_LOG(LogTemp, Warning, TEXT("Set crafter %p"), inCrafter);
	if (Crafter == inCrafter)
		return;

	Crafter = inCrafter;
	if (Crafter)
	{
		Crafter->CrafterListener = this;

		UE_LOG(LogTemp, Warning, TEXT("Add crafter recipies"), inCrafter);
		for (auto& r : Crafter->CraftableItems)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - recipie %s"), *r.CraftRecipieId);
			auto ItemDb = OmaUtil::GetItemDb(GetGameInstance());
			if (ItemDb)
			{
				auto rec = ItemDb->FindRecipie(r.CraftRecipieId);
				if (rec)
					AddRecipie(*rec);
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No recipie %s in ItemDb"), *r.CraftRecipieId);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No ItemDb in CrafterWidget"));
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Add crafter works"), inCrafter);
		for (auto &w : Crafter->Works)
		{
			w.Slot = AddWork(w);
		}
		CrafterSet(Crafter);
	}
	else
	{
		CloseCrafter();
	}
}


void
UCrafterWidget::WorkStarted(FCraftingWork& inWork)
{
	AddWork(inWork);
}

void
UCrafterWidget::WorkCompleted(int Id)
{
	RemoveWorkSlot(Id);
}

void
UCrafterWidget::SlotAdded_Implementation(UCrafterSlot* inSlot)
{
}

void
UCrafterWidget::SlotRemoved_Implementation(UCrafterSlot* inSlot)
{
}

void
UCrafterWidget::CrafterSet_Implementation(UCrafter* inCrafter)
{
}

void
UCrafterWidget::CrafterClosed_Implementation()
{
}

void
UCrafterWidget::WorkSlotAdded_Implementation(UCrafterSlot* inSlot)
{
}

void
UCrafterWidget::WorkSlotRemoved_Implementation(UCrafterSlot* inSlot)
{
}


void
UCrafterWidget::CloseCrafter()
{
	UE_LOG(LogTemp, Warning, TEXT("Close crafter UI"));
	if (Crafter)
		Crafter->CrafterListener = nullptr;
	CrafterClosed();
	Slots.Empty();
}

const FItemStruct*
UCrafterWidget::FindItem(const FString& Id) const
{
	auto ItemDb = OmaUtil::GetItemDb(GetGameInstance());
	if (ItemDb)
		return ItemDb->FindItem(Id);
	else
		return nullptr;
}

UCrafterSlot*
UCrafterWidget::AddRecipie(const FCraftRecipie& inRecipie)
{
	UE_LOG(LogTemp, Warning, TEXT("Add craftable item %s in recipie %s"), *inRecipie.CraftedItemId, *inRecipie.Id);

	auto SlotIndex = Slots.Num();
	FString name = FString::Format(TEXT("CrafterSlot/{0}/{1} recipie {2}"), { SlotIndex, *GetName(), *inRecipie.Id });

	auto item = FindItem(inRecipie.CraftedItemId);
	if (item)
	{
		auto slot = CreateWidget<UCrafterSlot>(this, CrafterSlotClass, *name);
		if (slot)
		{
			slot->Crafter = Crafter;
			slot->SlotIndex = SlotIndex;
			slot->SetSlot(inRecipie, *item);
			Slots.Add(slot);
			SlotAdded(slot);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to create crafter slot due missing CrafterSlotClass"));
		}
		return slot;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find craftable item %s in recipie %s"), *inRecipie.CraftedItemId, *inRecipie.Id);
	}
	return nullptr;
}

UCrafterSlot*
UCrafterWidget::AddWork(const FCraftingWork& inWork)
{
	UE_LOG(LogTemp, Warning, TEXT("Add crafing work %d in recipie %s"), inWork.Id, *inWork.Recipie.Id);

	FString name = FString::Format(TEXT("CrafterWork/{0}/{1} recipie {2}"), { inWork.Id, *GetName(), *inWork.Recipie.Id });

	auto item = FindItem(inWork.Recipie.CraftedItemId);
	if (item)
	{
		auto slot = CreateWidget<UCrafterSlot>(this, CrafterSlotClass, *name);
		if (slot)
		{
			slot->Crafter = Crafter;
			slot->SlotIndex = inWork.Id;
			slot->SetSlot(inWork.Recipie, *item);
			WorkSlots.Add(slot);
			UE_LOG(LogTemp, Warning, TEXT("Started work %d"), slot->SlotIndex);
			WorkSlotAdded(slot);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to create crafter work slot due missing CrafterSlotClass"));
		}
		return slot;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find craftable item %s in recipie %s"), *inWork.Recipie.CraftedItemId, *inWork.Recipie.Id);
	}
	return nullptr;
}

void
UCrafterWidget::RemoveSlot(int Index)
{
	auto slot = Slots[Index];
	Slots.RemoveAt(Index);
	SlotRemoved(slot);
}

void
UCrafterWidget::RemoveWorkSlot(int Id)
{
	for (int i = 0; i < WorkSlots.Num(); i++)
	{
		auto slot = WorkSlots[i];
		if (slot->SlotIndex == Id)
		{
			UE_LOG(LogTemp, Warning, TEXT("Completed work %d"), slot->SlotIndex);
			WorkSlotRemoved(slot);
			WorkSlots.RemoveAt(i);
			return;
		}
	}
}