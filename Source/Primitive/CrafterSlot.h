// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Craftable.h"
#include "CrafterSlot.generated.h"


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

	UFUNCTION(BlueprintCallable) void SetSlot(const FCraftRecipie& inSlot, const FItemStruct& inItem);

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY() class UInventory* Inventory = nullptr;
	UPROPERTY() int SlotIndex = 0;
	UPROPERTY() class UCrafter* Crafter = nullptr;

	FCraftRecipie Recipie;
	FItemStruct Item;
};
