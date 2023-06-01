// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabase.h"
#include "ItemStruct.h"

UItemDatabase::UItemDatabase(const FObjectInitializer& Init)
{
	InitItemData();
}


void
UItemDatabase::InitItemData()
{
	FItemStruct longStick;
	longStick.Id = TEXT("LongStick");
	longStick.Name = TEXT("Long Stick");
	//longStick.ItemClass = TEXT("/Game/ThirdPerson/Items/Item_LongStick");
	AddRow(TEXT("LongStick"), longStick);
}