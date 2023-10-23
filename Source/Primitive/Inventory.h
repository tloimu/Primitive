// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemStruct.h"
#include "CoreMinimal.h"
#include "Inventory.generated.h"

USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	FItemSlot() {}
	FItemSlot(const BodyPart MustWearIn) { CanOnlyWearIn.Add(MustWearIn); }

	bool operator== (const FItemSlot rhs) const;

	bool MergeTo(FItemSlot& ToSlot, int inCount);
	bool CanMergeTo(FItemSlot& ToSlot) const;
	FItemSlot& SetCount(int inCount);
	FItemSlot& ChangeCountBy(int inCount);
	void NotifyChange() const; // Notify the possible InventoryListener of the changed slot content

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Index = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FItemStruct Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Count = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<BodyPart> CanOnlyWearIn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool ShouldRemoveWhenEmpty = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UInventory* Inventory = nullptr;
};

class IInventoryListener
{
public:
	virtual void SlotChanged(const FItemSlot& Slot) = 0;
	virtual void SlotRemoved(const FItemSlot& Slot) = 0;
	virtual void MaxSlotsChanged(int MaxSlots) = 0; // Does this need to differentiate between different inventories?

protected:
	~IInventoryListener() {};
};

class IInventoryOwner
{
public:
	virtual AInteractableActor* DropItem(const FItemStruct& inItem) = 0;
	virtual const FItemStruct* FindItem(const FString& inId) const = 0;
	virtual void PlaySoundCrafting(const FItemStruct& inItem) const = 0; // TODO: Refactor to more logical place instead - currently used by <UCrafter>

protected:
	~IInventoryOwner() {};
};

UCLASS(BlueprintType)
class UInventory : public UObject
{
	GENERATED_BODY()

public:
	UInventory();
	UInventory(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable) bool AddItem(const FItemStruct& item, int count = 1);
	UFUNCTION(BlueprintCallable) bool RemoveItem(const FItemStruct& item, int count = 1);
	UFUNCTION(BlueprintCallable) bool SetMaxSlots(int Count);
	UFUNCTION(BlueprintCallable) void Organize();
	UFUNCTION(BlueprintCallable) bool IsEmpty() const;

	UFUNCTION(BlueprintCallable) bool CanMergeWith(FItemSlot& ToSlot, FItemSlot &FromSlot) const;
	UFUNCTION(BlueprintCallable) void MergeWith(FItemSlot& ToSlot, FItemSlot& FromSlot, int count);
	UFUNCTION(BlueprintCallable) void SplitSlot(int Index);

	UFUNCTION(BlueprintCallable) virtual FItemSlot& GetSlotAt(int Index);

	void DropItemsFromSlot(FItemSlot &inSlot, int inCount);
	void DropItem(const FItemStruct& inItem);

	const FItemStruct* FindItem(const FString& inId) const;
	int CountItemsOf(const FString& inId) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FItemSlot> Slots;
	UPROPERTY(EditAnywhere) int MaxSlots;
	UPROPERTY(EditAnywhere) int CurrentSelectedSlotIndex = -1;

	IInventoryListener *InventoryListener = nullptr;
	IInventoryOwner* InventoryOwner = nullptr;

	FItemSlot NoneSlot;
};
