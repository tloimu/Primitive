// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameInstance.h"
#include "ItemDatabase.h"
#include "WorldGenOne.h"
#include "FoliageResource.h"
#include "InteractableActor.h"
#include "PrimitiveCharacter.h"
#include "ItemStruct.h"

#include <Voxel/Public/VoxelWorldInterface.h>
#include <Voxel/Public/VoxelWorld.h>
#include <Voxel/Public/VoxelData/VoxelSave.h>
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"

const FString DefaultSaveGameName("DefaultGameSave");
const FString QuickSaveGameName("GameSaveSlot0");


void
UPrimitiveGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Game Instance: Init"));

	SetupGameConfig();
}

void
UPrimitiveGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Warning, TEXT("Game Instance: Shutdown"));
}

void
UPrimitiveGameInstance::SetupGameConfig()
{
	if (ItemDb)
	{
		ItemDb->SetupItems();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Item Database found"));
	}
}

APrimitiveCharacter*
UPrimitiveGameInstance::GetPlayerCharacter() const
{
	auto playerCharacter = Cast<APrimitiveCharacter>(GetFirstLocalPlayerController(GetWorld())->GetCharacter());
	if (!playerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No player character found when loading saved game"));
	}
	return playerCharacter;
}


void
UPrimitiveGameInstance::GenerateWorld()
{
	if (VoxelWorld)
	{
		if (!VoxelWorld->IsCreated())
		{
			VoxelWorld->CreateWorld();
		}
	}
}

void
UPrimitiveGameInstance::GenerateFoilage()
{
	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	TActorIterator<AInstancedFoliageActor> foliageIterator2(GetWorld());
	while (foliageIterator2)
	{
		UE_LOG(LogTemp, Warning, TEXT("===> FoliageActor: %s"), *(foliageIterator2->GetName()));

		TArray<UInstancedStaticMeshComponent*> components;
		foliageIterator2->GetComponents<UInstancedStaticMeshComponent>(components);
		UInstancedStaticMeshComponent* meshComponent = nullptr;
		if (components.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("No foilage components found (empty array)"));
		}
		else
		{
			meshComponent = components[0];
		}

		for (auto& c : components)
		{
			auto cs = c->InstanceStartCullDistance;
			auto ce = c->InstanceEndCullDistance;
			UE_LOG(LogTemp, Warning, TEXT("Component: %s, cull %d .. %d"), *c->GetName(), cs, ce);
		}
		UE_LOG(LogTemp, Warning, TEXT("<--- FoliageActor: %s"), *(foliageIterator2->GetName()));
		++foliageIterator2;
	}


	TActorIterator<AInstancedFoliageActor> foliageIterator(GetWorld());
	if (foliageIterator)
	{
		AInstancedFoliageActor* foliageActor = *foliageIterator;

		if (WorldGenInstance)
		{
			if (DoGenerateFoliage)
				WorldGenInstance->GenerateFoilage(*foliageActor);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No World Generator Instance found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No foilage actor found"));
	}
}


void
UPrimitiveGameInstance::LoadGame(const FString& inPath)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Instance: LoadGame from %s"), *inPath);

	auto playerCharacter = GetPlayerCharacter();
	UInventory* Inventory = playerCharacter ? playerCharacter->GetInventory() : nullptr;
	UInventory* EquippedItems = playerCharacter ? playerCharacter->GetEquippedItems() : nullptr;

	if (UGameplayStatics::DoesSaveGameExist(inPath, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading game..."), *inPath);
		SavedGame = Cast<UPrimitiveSaveGame>(UGameplayStatics::LoadGameFromSlot(inPath, 0));
		if (SavedGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Version %d: "), SavedGame->Version);
			if (VoxelWorld)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Load Voxels"));
				if (!VoxelWorld->IsCreated())
					VoxelWorld->CreateWorld();
				UVoxelDataTools::LoadFromCompressedSave(VoxelWorld, SavedGame->Voxels);
			}

			for (auto& player : SavedGame->Players)
			{
				if (playerCharacter)
				{
					UE_LOG(LogTemp, Warning, TEXT("  Load player %s: "), *player.name);
					playerCharacter->SetActorTransform(player.transform);
					if (Inventory)
					{
						for (auto& savedSlot : player.slots)
						{
							UE_LOG(LogTemp, Warning, TEXT("    - slot %d: %d of %s"), savedSlot.slot, savedSlot.count, *savedSlot.id);
							auto& slot = Inventory->GetSlotAt(savedSlot.slot);
							SetSavedInventorySlot(savedSlot, slot);
						}
					}
					if (EquippedItems)
					{
						for (auto& wear : player.wear)
						{
							UE_LOG(LogTemp, Warning, TEXT("    - wear on %s: %s (%f)"), *wear.on, *wear.id, wear.health);
							SetSavedEquippedSlot(wear, *EquippedItems);
						}
					}
				}
			}

			for (auto& item : SavedGame->Items)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Load item %s: "), *item.id);
				SpawnSavedItem(item);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Game Instance: Failed to load game from %s"), *inPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Instance: No game save slot to load %s"), *inPath);
	}

	UE_LOG(LogTemp, Warning, TEXT("Loading game completed"), *inPath);
}

void
UPrimitiveGameInstance::SetupGame()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVoxelWorld::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		VoxelWorld = Cast<AVoxelWorld>(FoundActors[0]);
		UE_LOG(LogTemp, Warning, TEXT("VoxelWorld actor found"));
	}

	if (!AutoLoadGamePath.IsEmpty())
	{
		FString path = AutoLoadGamePath;
		AutoLoadGamePath.Empty();
		LoadGame(path);
		return;
	}

	auto pc = GetPlayerCharacter();
	FString saveName = DefaultSaveGameName;
	if (pc)
		saveName = pc->GetDefaultSaveName();

	if (UGameplayStatics::DoesSaveGameExist(saveName, 0))
	{
		LoadGame(saveName);
	}
	else
	{
		GenerateWorld();
		GenerateFoilage();
	}
}

void
UPrimitiveGameInstance::ResetWorldToSavedGame(const FString& inPath)
{
	AutoLoadGamePath = inPath;
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


void
UPrimitiveGameInstance::SaveGame(const FString& inPath)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Instance: SaveGame to %s"), *inPath);

	if (!SavedGame)
		SavedGame = Cast<UPrimitiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UPrimitiveSaveGame::StaticClass()));

	SavedGame->Players.Empty();
	SavedGame->Items.Empty();
	SavedGame->Version = 2;

	auto playerCharacter = GetPlayerCharacter();
	FSavedPlayer savedPlayer;
	if (playerCharacter)
	{
		savedPlayer.name = playerCharacter->GetName();
		savedPlayer.transform = playerCharacter->GetActorTransform();
		savedPlayer.health = 100.0f;
		savedPlayer.stamina = 100.0f;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No player character found when saving game"));
	}

	UInventory* Inventory = playerCharacter ? playerCharacter->GetInventory() : nullptr;
	UInventory* EquippedItems = playerCharacter ? playerCharacter->GetEquippedItems() : nullptr;

	if (Inventory)
	{
		for (auto& slot : Inventory->Slots)
		{
			if (slot.Count > 0)
			{
				FSavedInventorySlot savedSlot;
				savedSlot.id = slot.Item.Id;
				savedSlot.slot = slot.Index;
				savedSlot.count = slot.Count;
				savedSlot.health = 1.0f;
				savedPlayer.slots.Add(savedSlot);
			}
		}
	}

	if (EquippedItems)
	{
		for (auto& slot : EquippedItems->Slots)
		{
			if (slot.Count > 0)
			{
				if (slot.CanOnlyWearIn.Num() == 1)
				{
					FSavedWearables savedSlot;
					savedSlot.id = slot.Item.Id;
					savedSlot.wearOn = static_cast<uint32>(* slot.CanOnlyWearIn.begin()); // ???? Not nice as this assumes one and only one "CanOnlyWearIn"-entry
					savedSlot.health = 1.0f;
					savedPlayer.wear.Add(savedSlot);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid equipment slot %d wear-in count %d"), slot.Index, slot.CanOnlyWearIn.Num());
				}
			}
		}
	}

	if (playerCharacter)
	{
		SavedGame->Players.Add(savedPlayer);
	}

	for (FActorIterator actorIt(GetWorld()); actorIt; ++actorIt)
	{
		auto itemActor = Cast<AInteractableActor>(*actorIt);
		if (itemActor)
		{
			FSavedItem savedItem;
			savedItem.id = itemActor->Item.Id;
			savedItem.itemRefId = itemActor->GetUniqueID();
			savedItem.transform = itemActor->GetActorTransform();
			if (itemActor->Inventory)
			{
				for (auto& slot : itemActor->Inventory->Slots)
				{
					if (slot.Count > 0)
					{
						FSavedInventorySlot savedSlot;
						savedSlot.id = slot.Item.Id;
						savedSlot.count = slot.Count;
						savedSlot.health = 1.0f;
						savedItem.slots.Add(savedSlot);
					}
				}
			}

			if (itemActor->SupportedByItem)
			{
				savedItem.supportedBy = itemActor->SupportedByItem->GetUniqueID();
			}

			SavedGame->Items.Add(savedItem);
		}
	}

	if (VoxelWorld)
	{
		UVoxelDataTools::GetCompressedSave(VoxelWorld, SavedGame->Voxels);
	}

	UGameplayStatics::SaveGameToSlot(SavedGame, inPath, 0);
}


AInteractableActor*
UPrimitiveGameInstance::SpawnSavedItem(const FSavedItem& item)
{
	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;

	FTransform transform = item.transform;

	FVector location;
	if (item.location.Num() >= 3)
		location.Set(item.location[0], item.location[1], item.location[2]);
	else
		location = transform.GetLocation();
	FRotator rotation;
	if (item.rotation.Num() >= 3)
		rotation.Add(item.rotation[0], item.rotation[1], item.rotation[2]);
	else
		rotation = FRotator(transform.GetRotation());

	UE_LOG(LogTemp, Warning, TEXT("Spawn item %s to x= %f, y= %f, z=%f"), *item.id, location.X, location.Y, location.Z);

	auto playerCharacter = GetPlayerCharacter();
	auto itemInfo = FindItem(item.id);
	if (itemInfo)
	{
		auto itemActor = SpawnItem(*itemInfo, location, rotation, playerCharacter);
		if (itemActor && itemActor->Inventory)
		{
			SetSavedContainerSlots(itemActor->Inventory, item);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown item ID [%s] in save file"), *item.id);
	}
	return nullptr;
}


AInteractableActor*
UPrimitiveGameInstance::SpawnItem(const FItemStruct& Item, const FVector& inLocation, const FRotator& inRotation, APrimitiveCharacter* OwningPlayer)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;//  AdjustIfPossibleButAlwaysSpawn;
	if (Item.ItemClass->IsValidLowLevel())
	{
		auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(Item.ItemClass, inLocation, inRotation, SpawnInfo);
		check(itemActor);
		itemActor->Item = Item;
		if (Item.ContainedSlots)
		{
			auto inv = NewObject<UInventory>();
			check(inv);
			itemActor->Inventory = inv;
			inv->InventoryOwner = OwningPlayer;
			inv->SetMaxSlots(Item.ContainedSlots);
		}
		if (!Item.CraftableRecipies.IsEmpty())
		{
			auto crafter = NewObject<UCrafter>();
			check(crafter);
			itemActor->Crafter = crafter;
			crafter->CrafterName = Item.Name;
			if (itemActor->Inventory)
				crafter->Inventory = itemActor->Inventory;
			else
				crafter->Inventory = OwningPlayer ? OwningPlayer->GetInventory() : nullptr; // ???? CHECK: Memory managements - maybe refactor so the player inventory is accessed only when needed
			for (auto& rid : Item.CraftableRecipies)
			{
				FCraftableItem rec;
				rec.CraftRecipieId = rid;
				crafter->CraftableItems.Add(rec);
			}
		}
		return itemActor;
	}
	else
	{
		if (Item.ItemClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find class %s for item %s"), *Item.ItemClass->GetName(), *Item.Id);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find class for item %s"), *Item.Id);
		}
	}

	return nullptr;
}


void
UPrimitiveGameInstance::SetSavedInventorySlot(const FSavedInventorySlot& saved, FItemSlot& slot)
{
	auto itemSetting = FindItem(saved.id);
	if (itemSetting && itemSetting->ItemClass->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("  - add %d items to slot %d %s"), saved.count, saved.slot, *itemSetting->Id);
		slot.Item = *itemSetting;
		slot.SetCount(saved.count).NotifyChange();
	}
}

void
UPrimitiveGameInstance::SetSavedEquippedSlot(const FSavedWearables& savedSlot, UInventory& EquippedItems)
{
	if (savedSlot.on == TEXT("back"))
	{
		for (auto& es : EquippedItems.Slots)
		{
			if (es.CanOnlyWearIn.Contains(BodyPart::Back))
			{
				auto itemSetting = FindItem(savedSlot.id);
				if (itemSetting && itemSetting->ItemClass->IsValidLowLevel())
				{
					es.Inventory = &EquippedItems;
					es.Item = *itemSetting;
					es.SetCount(1).NotifyChange();
				}
				break;
			}
		}
		// SetSavedInventorySlot(savedSlot, Inventory->Back);
	}
	else if (savedSlot.on == TEXT("head"))
	{
		// SetSavedInventorySlot(savedSlot, Inventory->Head);
	}
}

void
UPrimitiveGameInstance::SetSavedContainerSlots(UInventory* inInventory, const FSavedItem& saved)
{
	auto containerItem = FindItem(saved.id);
	if (containerItem)
	{
		inInventory->SetMaxSlots(containerItem->ContainedSlots);
		for (const auto& savedSlot : saved.slots)
		{
			auto& slot = inInventory->GetSlotAt(savedSlot.slot);
			SetSavedInventorySlot(savedSlot, slot);
		}
	}
}

const FItemStruct*
UPrimitiveGameInstance::FindItem(const FString& Id) const
{
	if (ItemDb)
		return ItemDb->FindItem(Id);
	else
		return nullptr;
}
