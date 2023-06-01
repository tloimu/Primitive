// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableActor.h"
#include "ItemStruct.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRIMITIVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	UFUNCTION(BlueprintCallable) bool AddItem(AInteractableActor* item);
	UFUNCTION(BlueprintCallable) bool RemoveItem(AInteractableActor* item);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemAdded(AInteractableActor* item);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemRemoved(AInteractableActor* item);

protected:

	UPROPERTY(EditAnywhere) TArray<FItemStruct> Items;
	UPROPERTY(EditAnywhere) int32 Capacity;
};
