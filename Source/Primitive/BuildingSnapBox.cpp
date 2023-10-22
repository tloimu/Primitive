// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSnapBox.h"

FName GAME_TAG_BUILDING("Building");

UBuildingSnapBox::UBuildingSnapBox(const FObjectInitializer& ObjectInitializer): UBoxComponent(ObjectInitializer),
	PlaceRotation(0.0f, 0.0f, 0.0f),
	PlaceOffset(0.0f, 0.0f, 0.0f)
{
	ComponentTags.Add(GAME_TAG_BUILDING);
}
