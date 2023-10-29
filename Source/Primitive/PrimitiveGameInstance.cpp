// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveGameInstance.h"
#include "ItemDatabase.h"
#include "WorldGenOne.h"
#include "FoliageResource.h"
#include "GameSettings.h"
#include "InteractableActor.h"
#include "PrimitiveCharacter.h"

#include <Voxel/Public/VoxelWorldInterface.h>
#include <Voxel/Public/VoxelWorld.h>
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
#include "Kismet/GameplayStatics.h"

void
UPrimitiveGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Game Instance: Init"));

	SavedGamePath = TEXT("PrimitiveSaveGame0");

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

void
UPrimitiveGameInstance::GenerateWorld()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVoxelWorld::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		VoxelWorld = Cast<AVoxelWorld>(FoundActors[0]);
		UE_LOG(LogTemp, Warning, TEXT("VoxelWorld actor found"));
	}
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
}

void
UPrimitiveGameInstance::SaveGame(const FString& inPath)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Instance: SaveGame to %s"), *inPath);
}


void
UPrimitiveGameInstance::ReadGameSave()
{
	const FString JsonFilePath = FPaths::ProjectContentDir() + "ConfigFiles/save-base.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*JsonFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("No save-file found in %s"), *JsonFilePath);
		return;
	}

	auto playerCharacter = Cast<APrimitiveCharacter>(GetFirstLocalPlayerController(GetWorld())->GetCharacter());
	if (!playerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No player character found when loading saved game"));
	}

	UInventory* Inventory = playerCharacter ? playerCharacter->GetInventory() : nullptr;
	UInventory* EquippedItems = playerCharacter ? playerCharacter->GetEquippedItems() : nullptr;

	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FGameSave game;
		FJsonObjectConverter::JsonObjectStringToUStruct<FGameSave>(JsonString, &game, 0, 0);
		UE_LOG(LogTemp, Warning, TEXT("Game Save Name= %s"), *game.name);
		for (const auto& item : game.items)
		{
			UE_LOG(LogTemp, Warning, TEXT("Saved item Id= %s, x= %f, y= %f, z=%f"), *item.id, item.location[0], item.location[1], item.location[2]);
			SpawnSavedItem(item);
		}

		if (Inventory && EquippedItems)
		{
			for (const auto& player : game.players)
			{
				if (player.name.Equals("one"))
				{
					for (const auto& savedSlot : player.slots)
					{
						auto& slot = Inventory->GetSlotAt(savedSlot.slot);
						SetSavedInventorySlot(savedSlot, slot);
					}
					for (const auto& savedSlot : player.wear)
					{
						if (savedSlot.on == TEXT("back"))
						{
							if (EquippedItems)
							{
								for (auto& es : EquippedItems->Slots)
								{
									if (es.CanOnlyWearIn.Contains(BodyPart::Back))
									{
										auto itemSetting = FindItem(savedSlot.id);
										if (itemSetting && itemSetting->ItemClass->IsValidLowLevel())
										{
											es.Inventory = EquippedItems;
											es.Item = *itemSetting;
											es.SetCount(1).NotifyChange();
										}
										break;
									}
								}
							}
							// SetSavedInventorySlot(savedSlot, Inventory->Back);
						}
						else if (savedSlot.on == TEXT("head"))
						{
							// SetSavedInventorySlot(savedSlot, Inventory->Head);
						}
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON at game save file %s"), *JsonString);
	}
}

AInteractableActor*
UPrimitiveGameInstance::SpawnSavedItem(const FSavedItem& item)
{
	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	FVector location;
	if (item.location.Num() >= 3)
		location.Set(item.location[0], item.location[1], item.location[2]);
	FRotator rotation;
	if (item.rotation.Num() >= 3)
		rotation.Add(item.rotation[0], item.rotation[1], item.rotation[2]);

	if (WorldGenInstance)
	{
		// Ensure not under ground
		auto l = location / WorldGenInstance->VoxelSize;
		auto th = WorldGenInstance->GetTerrainHeight(l.X, l.Y, l.Z);
		location.Z = th * WorldGenInstance->VoxelSize + 120.0f;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("World Generator not set yet to spawn items to"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawn item %s to x= %f, y= %f, z=%f"), *item.id, location.X, location.Y, location.Z);

	auto playerCharacter = Cast<APrimitiveCharacter>(GetFirstLocalPlayerController(GetWorld())->GetCharacter());

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
