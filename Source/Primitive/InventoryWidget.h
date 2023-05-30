#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainer.h"
#include "InteractableActor.h"
#include "InventoryWidget.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable) bool AddItem(AInteractableActor* item);
	UFUNCTION(BlueprintCallable) bool RemoveItem(AInteractableActor* item);
	UFUNCTION(BlueprintCallable) const TArray<AInteractableActor*> GetItems() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemAdded(AInteractableActor* item);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void ItemRemoved(AInteractableActor* item);

protected:

	UPROPERTY(EditAnywhere) TArray<AInteractableActor*> Items;
	UPROPERTY(EditAnywhere) int32 Capacity;
};
