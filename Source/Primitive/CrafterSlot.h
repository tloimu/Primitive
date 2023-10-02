// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Craftable.h"
#include <Containers/Map.h>
#include "CrafterSlot.generated.h"


USTRUCT(BlueprintType)
struct FCraftingHelpIngredient
{
	GENERATED_BODY()

	FCraftingHelpIngredient() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString	Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int	Available;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int	Needed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool	IsSufficient;
};


USTRUCT(BlueprintType)
struct FCraftingHelpInfo
{
	GENERATED_BODY()

	FCraftingHelpInfo() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FItemStruct	Item;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool	CanCraft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int	CanCraftCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FCraftingHelpIngredient> AvailableIngredients;
};


UCLASS(MinimalAPI, Blueprintable)
class UCrafterSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UCrafterSlot(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotSet(const FCraftRecipie& inSlot, const FItemStruct &inItem);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SetProgress(float Progress);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SetHighlight(bool DoHighlight);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotRemoved();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SetHelpText(const FCraftingHelpInfo &inInfo);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void HideHelpText();

	UFUNCTION(BlueprintCallable) void SetSlot(const FCraftRecipie& inSlot, const FItemStruct& inItem);
	UFUNCTION(BlueprintCallable) void SetSlotHelp();

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY() class UInventory* Inventory = nullptr;
	UPROPERTY() int SlotIndex = 0;
	UPROPERTY() class UCrafter* Crafter = nullptr;

	FCraftRecipie Recipie;
	FItemStruct Item;
};
