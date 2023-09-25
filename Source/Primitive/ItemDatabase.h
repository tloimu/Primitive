// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemStruct.h"
#include "ItemDatabase.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UItemDatabase(const FObjectInitializer& Init);

	UPROPERTY(EditAnywhere) TArray<FItemStruct> Items;
};
