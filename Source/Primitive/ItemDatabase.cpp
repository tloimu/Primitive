// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabase.h"
#include "InteractableActor.h"
#include "ItemStruct.h"

UItemDatabase::UItemDatabase(const FObjectInitializer& Init): UDataAsset(Init)
{
}

void
UItemDatabase::SetupItems()
{
	for (auto& spec : ItemSpecs)
	{
		if (spec.ItemClass->IsValidLowLevel())
		{
			auto o = spec.ItemClass.GetDefaultObject();
			o->Item.Id = spec.Id;
			Items.Add(spec.Id, o->Item);
		}
	}

	for (auto itemPair : Items)
	{
		auto &item = itemPair.Value;
		item.Icon.LoadSynchronous();
		UE_LOG(LogTemp, Warning, TEXT("ItemDb: %s %s %s"), *item.Id, *item.Icon.GetAssetName(), *item.ItemClass->GetClass()->GetName());
	}

	for (auto& spec : RecipieSpecs)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDb: Recipie %s"), *spec.Id);
		Recipies.Add(spec.Id, spec);
	}
}


const FItemStruct*
UItemDatabase::FindItem(const FString& Id) const
{
	return Items.Find(Id);
}

const FCraftRecipie*
UItemDatabase::FindRecipie(const FString& Id) const
{
	return Recipies.Find(Id);
}
