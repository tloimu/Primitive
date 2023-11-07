#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemStruct.h"
#include "CrafterSlot.h"
#include "Inventory.h"
#include "Craftable.h"
#include "CrafterWidget.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class UCrafterWidget : public UUserWidget, public ICrafterListener
{
	GENERATED_BODY()

public:

	UCrafterWidget(const FObjectInitializer& ObjectInitializer);

	void SetCrafter(UCrafter *inCrafter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CrafterSet(UCrafter *inCrafter);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotAdded(UCrafterSlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void SlotRemoved(UCrafterSlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void CrafterClosed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void WorkSlotAdded(UCrafterSlot* inSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable) void WorkSlotRemoved(UCrafterSlot* inSlot);

	UPROPERTY(EditAnywhere) TArray<UCrafterSlot*> Slots;
	UPROPERTY(EditAnywhere) TArray<UCrafterSlot*> WorkSlots;
	UPROPERTY(EditAnywhere) TSubclassOf<UCrafterSlot> CrafterSlotClass;

	UCrafterSlot* AddRecipie(const FCraftRecipie& inRecipie);
	UCrafterSlot* AddWork(const FCraftingWork& inWork);
	void CloseCrafter();
	void RemoveSlot(int Index);
	void RemoveWorkSlot(int Id);

	const FItemStruct* FindItem(const FString& Id) const;

	UPROPERTY(BlueprintReadOnly) class UCrafter* Crafter = nullptr;

	// <ICrafterListener>
	void WorkStarted(FCraftingWork& inWork) override;
	void WorkCompleted(int Id) override;
};
