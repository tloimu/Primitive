// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventorySlotDragOperation.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UInventorySlotDragOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UInventorySlot* WidgetReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D DragOffset;
};
