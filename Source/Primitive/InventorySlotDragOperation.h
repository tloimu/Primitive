// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotDragOperation.generated.h"

UCLASS()
class PRIMITIVE_API UDraggedInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void UpdateDragged(const FString &txt, const TSoftObjectPtr<UTexture> &icn);

	UFUNCTION(BlueprintCallable) void SetInventorySlot(class UInventorySlot* inSlot);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UInventorySlot* FromSlot = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString SizeText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D DragOffset;
};


UCLASS()
class PRIMITIVE_API UInventorySlotDragOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UDraggedInventorySlot* InventorySlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D DragOffset;
};
