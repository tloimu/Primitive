#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "ItemContainer.generated.h"

/*
UCLASS()
class PRIMITIVE_API AItemContainer : public AInteractableActor
{
	GENERATED_BODY()

	AItemContainer();

public:

	UFUNCTION(BlueprintCallable) bool AddItem(UItemSpecs* item);
	UFUNCTION(BlueprintCallable) bool RemoveItem(UItemSpecs* item);
	UFUNCTION(BlueprintCallable) const TArray<UItemSpecs*> GetItems() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemAdded(UItemSpecs* item);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemRemoved(UItemSpecs* item);

protected:
	UPROPERTY(EditAnywhere) TArray<UItemSpecs*> Items;
	UPROPERTY(EditAnywhere) int32 Capacity;
};
*/