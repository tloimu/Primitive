// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDatabase.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UItemDatabase : public UDataTable
{
	GENERATED_BODY()

	UItemDatabase(const FObjectInitializer& Init);

	UFUNCTION(BlueprintCallable) void InitItemData();
};
