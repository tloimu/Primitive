// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemStruct.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Craftable.generated.h"


USTRUCT(BlueprintType)
struct FCraftIngredient
{
	GENERATED_BODY()

	FCraftIngredient() {}

	bool operator== (const FCraftIngredient&rhs) const
	{
		if (ItemId == rhs.ItemId && ItemCount == rhs.ItemCount && MinimumQuality == rhs.MinimumQuality)
		{
			return true;
		}
		else return false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString	ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int		ItemCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float	MinimumQuality = 0.5f;
};

USTRUCT(BlueprintType)
struct FCraftRecipie
{
	GENERATED_BODY()

	FCraftRecipie() {}

	bool operator== (const FCraftRecipie &rhs) const
	{
		if (Id == rhs.Id && Quality == rhs.Quality && Ingredients == rhs.Ingredients)
		{
			return true;
		}
		else return false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString	Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString	CraftedItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int		CraftedItemCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float	CraftingSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float	Quality = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCraftIngredient>	Ingredients;
};

USTRUCT(BlueprintType)
struct FCraftableItem
{
	GENERATED_BODY()

	FCraftableItem() {}

	bool operator== (const FCraftableItem &rhs) const
	{
		if (CraftRecipieId == rhs.CraftRecipieId && Quality == rhs.Quality)
		{
			return true;
		}
		else return false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString		CraftRecipieId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float		Quality = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float		Efficiency = 1.0f;
};

USTRUCT(BlueprintType)
struct FCraftingWork
{
	GENERATED_BODY()

	FCraftingWork() {}

	bool operator== (const FCraftingWork&rhs) const
	{
		if (Id == rhs.Id)
		{
			return true;
		}
		else return false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int				Id = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCraftRecipie	Recipie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UCrafterSlot	*Slot = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float			GameTimeProgressLeft = 0.0f;
};

class ICrafterListener
{
public:
	virtual void WorkStarted(FCraftingWork &inWork) = 0;
	virtual void WorkCompleted(int Id) = 0;
};

UCLASS(BlueprintType)
class UCrafter : public UObject
{
	GENERATED_BODY()

public:
	UCrafter();
	UCrafter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CraftingStarted(const FCraftingWork &inWork);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CraftingProgressing(const FCraftingWork &inWork);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CraftingCompleted(const FCraftingWork &inWork);

	void Setup();
	
	bool CanCraft(const FCraftRecipie& inRecipie, TArray<class UInventory*> inIngredientInventories);

	bool StartCrafting(const FCraftRecipie& inRecipie, TArray<class UInventory*> inIngredientInventories, UCrafterSlot* inSlot);
	void CheckCrafting(float DeltaGameTimeSecs);
	void CompleteCrafting(FCraftingWork& inProgress);

	void PlaySoundCrafting(const UCrafterSlot* inSlot);

	UPROPERTY(BlueprintReadOnly) FString CrafterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCraftableItem>	CraftableItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float			Efficiency = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float			Quality = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCraftingWork>	Works;

	UPROPERTY() class UInventory *Inventory = nullptr;
	float InventoryRange = 500.0f; // cm

	int NextWorkId = 1;

	static const FString HandCraftingStationItemId;

	ICrafterListener* CrafterListener = nullptr;
};
