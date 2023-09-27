// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabase.h"
#include "ItemStruct.h"

UItemDatabase::UItemDatabase(const FObjectInitializer& Init): UDataAsset(Init)
{
}

const FItemStruct*
UItemDatabase::FindItem(const FString& Id) const
{
	for (auto& item : Items)
	{
		if (item.Id == Id)
			return &item;
	}
	return nullptr;
}
