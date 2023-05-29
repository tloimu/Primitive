#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "ItemContainer.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API AItemContainer : public AInteractableActor
{
	GENERATED_BODY()

	AItemContainer();

public:

	UFUNCTION(BlueprintCallable) bool AddItem(AInteractableActor* item);
	UFUNCTION(BlueprintCallable) bool RemoveItem(AInteractableActor* item);
	UFUNCTION(BlueprintCallable) const TArray<AInteractableActor*> GetItems() const;

protected:
	UPROPERTY(EditAnywhere) TArray<AInteractableActor*> Items;
	UPROPERTY(EditAnywhere) int32 Capacity;
};
