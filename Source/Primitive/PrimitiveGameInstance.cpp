// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameInstance.h"
#include "ItemDatabase.h"
#include "WorldGenOne.h"
#include "FoliageResource.h"
#include "InteractableActor.h"
#include "PrimitiveCharacter.h"
#include "ItemStruct.h"
#include "OmaUtil.h"
#include "HISMFoliageActor.h"

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


TArray<UInstancedStaticMeshComponent*>
UPrimitiveGameInstance::GetFoliageComponents()
{
	TArray<UInstancedStaticMeshComponent*> components;
	TActorIterator<AInstancedFoliageActor> foliageIterator2(GetWorld());
	while (foliageIterator2)
	{
		UE_LOG(LogTemp, Warning, TEXT("===> FoliageActor: %s"), *(foliageIterator2->GetName()));
		foliageIterator2->GetComponents<UInstancedStaticMeshComponent>(components);
		++foliageIterator2;
	}
	return components;
}

TArray<UHierarchicalInstancedStaticMeshComponent*>
UPrimitiveGameInstance::GetHismFoliageComponents()
{
	TArray<UHierarchicalInstancedStaticMeshComponent*> components;
	TActorIterator<AHISMFoliageActor> foliageIterator2(GetWorld());
	while (foliageIterator2)
	{
		foliageIterator2->GetComponents<UHierarchicalInstancedStaticMeshComponent>(components);
		UE_LOG(LogTemp, Warning, TEXT("===> FoliageActor: %s, components %d"), *(foliageIterator2->GetName()), components.Num());
		++foliageIterator2;
	}
	return components;
}

void
UPrimitiveGameInstance::GenerateTerrain()
{
	UE_LOG(LogTemp, Warning, TEXT("Generate Voxel World"));
	if (VoxelWorld)
	{
		if (!VoxelWorld->IsCreated())
		{
			VoxelWorld->CreateWorld();
		}
	}
}

void
UPrimitiveGameInstance::GenerateFoliage()
{
	UE_LOG(LogTemp, Warning, TEXT("Generate Foliage"));
	DestroyAllItemsAndResources(false, true);

	auto components = GetHismFoliageComponents();
	for (auto& c : components)
	{
		auto cs = c->InstanceStartCullDistance;
		auto ce = c->InstanceEndCullDistance;
		UE_LOG(LogTemp, Warning, TEXT("Component: %s, cull %d .. %d, instances %ld"), *c->GetName(), cs, ce, c->GetInstanceCount());
	}

	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	if (WorldGenInstance)
	{
		if (DoGenerateFoliage)
		{
			TArray<UInstancedStaticMeshComponent*> components2;
			for (auto c : components)
				components2.Push(c);
			WorldGenInstance->GenerateFoliage(components2);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No World Generator Instance found"));
	}
}


void
UPrimitiveGameInstance::LoadGame(const FString& inPath)
{
	OmaUtil::MsTimer timer;

	UE_LOG(LogTemp, Warning, TEXT("Game Instance: LoadGame from %s"), *inPath);

	if (UGameplayStatics::DoesSaveGameExist(inPath, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading game..."), *inPath);
		auto SavedGame = Cast<UPrimitiveSaveGame>(UGameplayStatics::LoadGameFromSlot(inPath, 0));
		if (SavedGame)
		{
			timer.LogAndCheck("load data from slot");

			UE_LOG(LogTemp, Warning, TEXT("  Version %d: "), SavedGame->Version);

			// Load terrain
			if (VoxelWorld)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Load Voxels"));
				if (!VoxelWorld->IsCreated())
					VoxelWorld->CreateWorld();
				UVoxelDataTools::LoadFromCompressedSave(VoxelWorld, SavedGame->Voxels);
			}
			timer.LogAndCheck("load voxels");

			// Load players
			for (auto& player : SavedGame->Players)
			{
				auto playerCharacter = GetPlayerCharacter(); // TODO: In multiplayer, this should be the individual player in question
				UInventory* Inventory = playerCharacter ? playerCharacter->GetInventory() : nullptr;
				UInventory* EquippedItems = playerCharacter ? playerCharacter->GetEquippedItems() : nullptr;

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
			timer.LogAndCheck("load players");

			// Load resources
			TArray<TArray<FTransform> > NewInstancesPerComponent;
			auto foliageComponents = GetHismFoliageComponents();
			for (auto& c : foliageComponents)
			{
				NewInstancesPerComponent.AddDefaulted();
				NewInstancesPerComponent.Reserve(MaxFoliageInstances);
			}

			UE_LOG(LogTemp, Warning, TEXT("  Loading %ld resources"), SavedGame->Resources.Num());			
			for (auto& i : SavedGame->Resources)
			{
				if (i.id < NewInstancesPerComponent.Num())
				{
					NewInstancesPerComponent[i.id].Add(i.transform);
				}
			}
			uint32 ci = 0;
			for (auto& c : foliageComponents)
			{
				c->AddInstances(NewInstancesPerComponent[ci], false);
				UE_LOG(LogTemp, Warning, TEXT("  Loaded %ld resource %s"), NewInstancesPerComponent[ci].Num(), *c->GetName());
				ci++;
			}
			timer.LogAndCheck("load resources");

			// Load items
			TMap<uint32, TObjectPtr<AInteractableActor>> refIdToItem;
			refIdToItem.Reserve(SavedGame->Items.Num());
			for (auto& item : SavedGame->Items)
			{
				UE_LOG(LogTemp, Warning, TEXT("  Load item %s: "), *item.id);
				auto itemActor = SpawnSavedItem(item);
				if (itemActor)
					refIdToItem.Add(item.itemRefId, itemActor);
			}
			timer.LogAndCheck("load items");

			for (auto& item : SavedGame->Items)
			{
				if (item.supportedBy)
				{
					auto supportingItem = refIdToItem.Find(item.supportedBy);
					auto supportedItem = refIdToItem.Find(item.itemRefId);
					if (supportedItem && supportingItem)
					{
						supportedItem->Get()->SupportedByItem = supportingItem->Get();
						supportingItem->Get()->SupportsItems.Add(supportedItem->Get());
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("  Invalid support info item=%ld supportedBy=%ld: "), item.itemRefId, item.supportedBy);
					}
				}
			}

			timer.LogAndCheck("resolve item references");

			for (auto& item : refIdToItem)
			{
				if (item.Value)
				{
					item.Value->OnLoaded();
				}
			}

			timer.LogAndCheck("OnLoaded called");

			timer.LogTotal("load game");
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
		GenerateTerrain();
		GenerateFoliage();
	}
}

void
UPrimitiveGameInstance::ResetWorldToSavedGame(const FString& inPath)
{
	AutoLoadGamePath = inPath;
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


void
UPrimitiveGameInstance::DestroyAllItemsAndResources(bool inItems, bool inResources)
{
	if (inResources)
	{
		TArray<UInstancedStaticMeshComponent*> components;
		TActorIterator<AInstancedFoliageActor> foliageIterator2(GetWorld());
		while (foliageIterator2)
		{
			foliageIterator2->GetComponents<UInstancedStaticMeshComponent>(components);
			++foliageIterator2;
		}
		for (auto c : components)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - destroying %ld or %s"), c->GetInstanceCount(), *c->GetName());
			while (c->GetInstanceCount() > 0)
				c->RemoveInstance(0);
		}
	}

	if (inItems)
	{
		TArray<AActor*> destroyAll;

		for (TActorIterator<AInteractableActor> it(GetWorld()); it; ++it)
		{
			destroyAll.Add(*it);
		}
		UE_LOG(LogTemp, Warning, TEXT("  - destroying %ld items"), destroyAll.Num());
		for (auto a : destroyAll)
			a->Destroy();
	}
}


void
UPrimitiveGameInstance::SaveGame(const FString& inPath)
{
	OmaUtil::MsTimer timer;

	UE_LOG(LogTemp, Warning, TEXT("Game Instance: SaveGame to %s"), *inPath);

	//if (!SavedGame)
	auto SavedGame = Cast<UPrimitiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UPrimitiveSaveGame::StaticClass()));

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

	timer.LogAndCheck("save players");

	for (FActorIterator actorIt(GetWorld()); actorIt; ++actorIt)
	{
		auto itemActor = Cast<AInteractableActor>(*actorIt);
		if (itemActor)
		{
			FSavedItem savedItem;
			savedItem.id = itemActor->Item.Id;
			savedItem.itemRefId = itemActor->GetUniqueID();
			savedItem.transform = itemActor->GetActorTransform();
			savedItem.state = itemActor->CurrentState;
			if (itemActor->Inventory)
			{
				for (auto& slot : itemActor->Inventory->Slots)
				{
					if (slot.Count > 0)
					{
						FSavedInventorySlot savedSlot;
						savedSlot.id = slot.Item.Id;
						savedSlot.slot = slot.Index;
						savedSlot.count = slot.Count;
						savedSlot.health = 1.0f;
						savedItem.slots.Add(savedSlot);
						UE_LOG(LogTemp, Warning, TEXT("  - saving item slot %d of %s"), slot.Index, *slot.Item.Id);
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

	timer.LogAndCheck("save actors");

	if (VoxelWorld)
	{
		UVoxelDataTools::GetCompressedSave(VoxelWorld, SavedGame->Voxels);
		timer.LogAndCheck("save voxels");
	}

	int32 ci = 0;
	uint32 icount = 0;
	for (auto& c : GetHismFoliageComponents())
	{
		FSavedResource saved;
		saved.id = ci;
		UE_LOG(LogTemp, Warning, TEXT("  instancemap %d, instances %d"), c->PerInstanceSMData.Num(), c->PerInstanceIds.Num());
		for (auto i = 0; i < c->PerInstanceSMData.Num(); i++)
		{
			if (c->GetInstanceTransform(i, saved.transform))
			{
				SavedGame->Resources.Add(saved);
				icount++;
			}
		}
		ci++;
	}
	UE_LOG(LogTemp, Warning, TEXT("Saved %ld resources"), icount);


	timer.LogAndCheck("save resources");

	UGameplayStatics::SaveGameToSlot(SavedGame, inPath, 0);

	timer.LogAndCheck("save data to slot");

	timer.LogTotal("save game");
}


AInteractableActor*
UPrimitiveGameInstance::SpawnSavedItem(const FSavedItem& item)
{
	auto location = item.transform.GetLocation();
	UE_LOG(LogTemp, Warning, TEXT("Spawn item %s to x= %f, y= %f, z=%f"), *item.id, location.X, location.Y, location.Z);

	auto playerCharacter = GetPlayerCharacter();
	auto itemInfo = FindItem(item.id);
	if (itemInfo)
	{
		auto itemActor = SpawnItem(*itemInfo, item.transform, playerCharacter);
		if (itemActor)
			itemActor->CurrentState = item.state;
		if (itemActor && itemActor->Inventory)
		{
			SetSavedContainerSlots(itemActor->Inventory, item);
		}
		return itemActor;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown item ID [%s] in save file"), *item.id);
	}
	return nullptr;
}


AInteractableActor*
UPrimitiveGameInstance::SpawnItem(const FItemStruct& Item, const FTransform& inTransform, APrimitiveCharacter* OwningPlayer)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;//  AdjustIfPossibleButAlwaysSpawn;
	if (Item.ItemClass->IsValidLowLevelFast())
	{
		auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(Item.ItemClass, inTransform, SpawnInfo);
		check(itemActor);
		itemActor->Item = Item;
		if (Item.ContainedSlots)
		{
			auto inv = NewObject<UInventory>();
			check(inv);
			itemActor->Inventory = inv;
			inv->InventoryOwner = OwningPlayer;
			inv->SlotCapability = itemActor->Item.SlotCapability;
			inv->SlotUtility = itemActor->Item.SlotUtility;
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
			if (es.CanOnlyWearIn.Contains(EBodyPart::Back))
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
