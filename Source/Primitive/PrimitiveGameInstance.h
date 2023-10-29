// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameSettings.h"
#include "Inventory.h"
#include "PrimitiveGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVE_API UPrimitiveGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	void Init() override;
	void Shutdown() override;

public:

	// Item Database

	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UItemDatabase* ItemDb = nullptr;
	const FItemStruct* FindItem(const FString& Id) const;


	// Saving and loading game

	void SetupGameConfig();

	void LoadGame(const FString& inPath);
	void SaveGame(const FString& inPath);

	void ReadGameSave();
	class AInteractableActor* SpawnSavedItem(const FSavedItem& item);
	class AInteractableActor* SpawnItem(const FItemStruct& Item, const FVector& inLocation, const FRotator& inRotation, class APrimitiveCharacter *OwningPlayer);
	void SetSavedInventorySlot(const FSavedInventorySlot& saved, FItemSlot& slot);
	void SetSavedContainerSlots(UInventory* inInventory, const FSavedItem& saved);

	FString SavedGamePath;


	// Setting up the Map

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool DoGenerateFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 MaxFoliageInstances = 200000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 MaxFoliageRange = 20000;

	void GenerateWorld();
	void GenerateFoilage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly) class AVoxelWorld* VoxelWorld = nullptr;
};
