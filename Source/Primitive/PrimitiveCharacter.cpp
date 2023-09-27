// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrimitiveCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "GameSettings.h"
#include "ItemDatabase.h"
#include "InstancedFoliageActor.h"
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
#include <Primitive/Interactable.h>
#include "WorldGenOne.h"
#include "PrimitiveGameMode.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "Kismet/GameplayStatics.h"

#include <Voxel/Public/VoxelTools/Gen/VoxelSphereTools.h>
#include <Voxel/Public/VoxelWorldInterface.h>
#include <Voxel/Public/VoxelWorld.h>
#include <Voxel/Public/VoxelTools/Gen/VoxelToolsBase.h>
#include <Voxel/Public/VoxelTools/VoxelDataTools.h>


//////////////////////////////////////////////////////////////////////////
// APrimitiveCharacter

APrimitiveCharacter::APrimitiveCharacter(): DoGenerateFoliage(true), ClockInSecs(12 * 60 * 60.0f), Day(1), DayOfYear(6 * 30), ClockSpeed(600.0f)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->PostProcessSettings.AutoExposureMaxBrightness = 1.0f;
	FollowCamera->PostProcessSettings.AutoExposureMinBrightness = 1.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Zoom level defaults
	ZoomTransforms.Add(-200.0f); // Actually, here we should really "zoom" instead of move camera forward
	ZoomTransforms.Add(-40.0f); // Would be nice to see hands here...
	ZoomTransforms.Add(80.0f);
	ZoomTransforms.Add(200.0f);
	ZoomTransforms.Add(400.0f);
	ZoomTransforms.Add(600.0f);
	ZoomTransforms.Add(800.0f);
	CurrentZoomLevel = 5;
	check(CurrentZoomLevel < ZoomTransforms.Num());
	CameraBoom->TargetArmLength = ZoomTransforms[CurrentZoomLevel];

	CurrentTarget = nullptr;
	CurrentTargetComponent = nullptr;
	CurrentTargetInstanceId = -1;
	ShowingInventory = false;
	TargetVoxelWorld = nullptr;

	InventoryWidgetClass = nullptr;
	InventoryWidget = nullptr;

	HUDWidgetClass = nullptr;
	HUDWidget = nullptr;

	WorldGeneratorClass = nullptr;

	Inventory = nullptr;

//	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void APrimitiveCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Inventory = NewObject<UInventory>();
	Inventory->Player = this;

	//Add Input Mapping Context
	auto pc = GetController<APlayerController>();
	if (pc)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		if (IsLocallyControlled() && InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(pc, InventoryWidgetClass);
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay: CreateInventoryWidget"));
			check(InventoryWidget);
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay: CreateInventoryWidget OK"));
			InventoryWidget->Inventory = Inventory;
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Inventory 1"));
			Inventory->InventoryListener = InventoryWidget;
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Inventory 2"));
			Inventory->SetMaxSlots(30);
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Inventory OK"));

			HUDWidget = CreateWidget<UHUDWidget>(pc, HUDWidgetClass);
			check(HUDWidget);
			HUDWidget->AddToPlayerScreen();
		}
	}

	ReadConfigFiles();
	ReadGameSave();

	EnsureNotUnderGround();

	GenerateFoilage();

	// Some validity checking
	if (!SunLight)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			SunLight = FoundActors[0];
			UE_LOG(LogTemp, Warning, TEXT("Surrogate SunLight actor found as the first ADirectionalLight in the level"));
		}
	}

	if (SunLight)
	{
		UE_LOG(LogTemp, Warning, TEXT("SunLight actor found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SunLight actor NOT found"));		
	}


}

void
APrimitiveCharacter::GenerateFoilage()
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
APrimitiveCharacter::EnsureNotUnderGround()
{
	// ???? TODO: move the player on top of the terrain if under ground at startup
}

void
APrimitiveCharacter::ReadConfigFiles()
{
	if (ItemDb)
	{
		for (auto& item : ItemDb->Items)
		{
			item.Icon.LoadSynchronous();
			UE_LOG(LogTemp, Warning, TEXT("ItemDb: %s %s %s"), *item.Id, *item.Icon.GetAssetName(), *item.ItemClass->GetClass()->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Item Database found"));
	}
}
/*
void
APrimitiveCharacter::UpdateItemSettingsClass(FItemSettings& item)
{
	TSoftClassPtr<AActor> ActorBpClass = TSoftClassPtr<AActor>(FSoftObjectPath(item.className));
	UClass* LoadedBpAsset = ActorBpClass.LoadSynchronous();
	if (LoadedBpAsset)
		LoadedBpAsset->AddToRoot();
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find item asset %s"), *item.className);
	}

	item.ItemClass = LoadedBpAsset;

	item.Icon = item.iconName;
	item.Icon.LoadSynchronous();
}
*/
/*
// First we need a reference to the BP at all - yes, hardcoded asset paths are not a nice thing,
// but it has to come from somewhere....

// ....if your blueprint is in a plugin
TSoftClassPtr<AActor> ActorBpClass = TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("Blueprint'/PluginModuleName/Some/Path/BP_MyAwesomeActor.BP_MyAwesomeActor_C'")));

// ....if your blueprint is in the normal project content
TSoftClassPtr<AActor> ActorBpClass = TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("Blueprint'/Game/Some/Path/BP_MyAwesomeActor.BP_MyAwesomeActor_C'")));

// The actual loading
UClass* LoadedBpAsset = ActorBpClass.LoadSynchronous();

// (Optional, depends on how you continue using it)
// Make sure GC doesn't steal it away from us, again
LoadedBpAsset->AddToRoot();

// From here on, it's business as usual, common actor spawning, just using the BP asset we loaded above
FVector Loc = FVector::ZeroVector;
FRotator Rot = FRotator::ZeroRotator;
FActorSpawnParameters SpawnParams = FActorSpawnParameters();
GetWorld()->SpawnActor(LoadedBpAsset, &Loc, &Rot, SpawnParams);
*/

void
APrimitiveCharacter::ReadGameSave()
{
	const FString JsonFilePath = FPaths::ProjectContentDir() + "ConfigFiles/save-base.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*JsonFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("No save-file found in %s"), *JsonFilePath);
		return;
	}

	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FGameSave game;
		FJsonObjectConverter::JsonObjectStringToUStruct<FGameSave>(JsonString, &game, 0, 0);
		UE_LOG(LogTemp, Warning, TEXT("Game Save Name= %s"), *game.name);
		for (const auto &item : game.items)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Settings Id= %s, x= %f, y= %f, z=%f"), *item.id, item.location[0], item.location[1], item.location[2]);
			SpawnItem(item);
		}
		if (Inventory)
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

void
APrimitiveCharacter::SpawnItem(const FSavedItem& item)
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

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;//  AlwaysSpawn;

	auto itemInfo = FindItem(item.id);
	if (itemInfo)
	{
		if (itemInfo->ItemClass->IsValidLowLevel())
			auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(itemInfo->ItemClass, location, rotation, SpawnInfo);
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find item class %s"), *itemInfo->ItemClass->GetName());
		}
	}
}

const FItemStruct*
APrimitiveCharacter::FindItem(const FString& Id) const
{
	if (ItemDb)
		return ItemDb->FindItem(Id);
	else
		return nullptr;
}

void
APrimitiveCharacter::SetSavedInventorySlot(const FSavedInventorySlot& saved, FItemSlot &slot)
{
	//auto itemSetting = ItemSettings.Find(saved.id);
	auto itemSetting = FindItem(saved.id);
	if (itemSetting && itemSetting->ItemClass->IsValidLowLevel())
	{
		slot.Item.Id = itemSetting->Id;
		slot.Item.ItemClass = itemSetting->ItemClass;
		slot.Item.Icon = itemSetting->Icon;
		slot.Item.MaxStackSize = itemSetting->MaxStackSize;
		slot.Item.MaxHealth = itemSetting->MaxHealth;
		slot.Item.Weight = itemSetting->Weight;
		slot.Count = saved.count;
		if (Inventory->InventoryListener)
			Inventory->InventoryListener->SlotChanged(saved.slot, slot);
	}
}

void
APrimitiveCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}


void APrimitiveCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckTarget();

	CheckEnvironment();
	CheckSunlight(DeltaSeconds);
}

void
APrimitiveCharacter::CheckEnvironment()
{
	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	if (WorldGenInstance)
	{
		if (WorldGenInstance->VoxelSize > 1.0f)//TargetVoxelWorld) // ???? TODO: Fix to get a permanenet handle to the "world" - maybe via GameInstance?
		{
			FIntVector l(GetActorLocation() / WorldGenInstance->VoxelSize);//TargetVoxelWorld->GlobalToLocal(GetActorLocation());
			auto th = WorldGenInstance->GetTerrainHeight(l.X, l.Y, l.Z);
			if (l.Z < th - 2.0f)// || GetActorLocation().Z < -100000.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Jumped UP %f"), WorldGenInstance->VoxelSize);
				FVector up = GetActorLocation();
				up.Z += FMath::Max(0.0f, th) * WorldGenInstance->VoxelSize + 5000.0f;
				SetActorLocation(up);
			}

			auto T = WorldGenInstance->GetTemperature(l.X, l.Y, l.Z);
			auto M = WorldGenInstance->GetMoisture(l.X, l.Y, l.Z);
			HUDWidget->SetEnvironment(T, M);
			float lat = WorldGenInstance->GetLatitude(l.Y);
			HUDWidget->SetLocation(l, lat);
			HUDWidget->SetTerrainHeight(th);
			HUDWidget->SetHealth(99.0f);
			HUDWidget->SetStamina(92.0f);
		}
		else
		{
			if (GetActorLocation().Z < -10000.f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Jumped up without TargetVoxelWorld"));
				FVector up = GetActorLocation();
				up.Z += 30000.0f;
				SetActorLocation(up);
			}
		}
	}
}

void
APrimitiveCharacter::CheckSunlight(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameMode::Tick %f"), DeltaSeconds);
	if (SunLight)
	{
		float clockAdvance = DeltaSeconds * ClockSpeed;
		if (FMath::TruncToInt(ClockInSecs / (60 * 60.0f)) != FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)))
		{
			UE_LOG(LogTemp, Warning, TEXT("APrimitiveGameMode::Hour = %d"), FMath::TruncToInt((ClockInSecs + clockAdvance) / (60 * 60.0f)));
		}
		ClockInSecs += clockAdvance;
		if (ClockInSecs > 24 * 60 * 60.0f)
		{
			Day++;
			DayOfYear++;
			ClockInSecs -= 24 * 60 * 60.0f;
			UE_LOG(LogTemp, Warning, TEXT("Start of Day %d"), Day);
		}

		FRotator rot;
		rot.Pitch = ClockInSecs * 360.0f / (24 * 60 * 60.0f) + 90.0f;
		SunLight->SetActorRotation(rot);
		if (SkyLight)
		{
		}
	}
}


void APrimitiveCharacter::CheckTarget()
{
	FHitResult hits;
	ECollisionChannel channel = ECollisionChannel::ECC_Visibility;
	FVector start = GetActorLocation();
	FVector end = start + FollowCamera->GetForwardVector().GetSafeNormal() * 500.0f;

	FCollisionQueryParams params;
	GetWorld()->LineTraceSingleByChannel(hits, start, end, channel, params);

	if (hits.bBlockingHit)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, hits.Location, FColor(100, 0, 0));
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), hits.Location, 5, 5, FLinearColor::White);

		auto hit = hits.GetActor();
		if (hit->Implements<UInteractable>())
		{
			SetCurrentTarget(hit);
		}
		else
		{
			auto voxels = Cast<AVoxelWorld>(hit);

			if (voxels)
			{
				TArray<FModifiedVoxelValue> OutModifiedValues;
				// UE_LOG(LogTemp, Warning, TEXT("Hit VoxelWorld target %s (%d)"), *hit->GetName(), rand());

				auto poses = voxels->GetNeighboringPositions(hits.Location);				

				if (poses.Num() > 0)
				{
					TargetVoxelWorld = voxels;
					TargetLocation = hits.Location;
					/*
					FVoxelMaterial Material;
					UVoxelDataTools::GetMaterial(Material, voxels, poses[0]);
					float Value;
					UVoxelDataTools::GetValue(Value, voxels, poses[0]);
					// UE_LOG(LogTemp, Warning, TEXT("Voxel Material n=%d, v=%f, A=%f, index=%d, ei=%d"), poses.Num(), Value, Material.GetA_AsFloat(), Material.GetSingleIndex(), hits.ElementIndex);
					int i = 0;
					CurrentVoxel = poses[0];
					for (auto& p : poses)
					{
						UVoxelDataTools::GetMaterial(Material, voxels, p);
						//UE_LOG(LogTemp, Warning, TEXT(" - Material[%d]:x=%d, y=%d, z=%d, index=%f"), i, p.X, p.Y, p.Z, Material.GetSingleIndex_AsFloat());
						for (int c = 0; c < FVoxelMaterial::NumChannels; c++)
						{
							auto materialIndex = Material.GetRaw(c);
							if (materialIndex)
							{
								// UE_LOG(LogTemp, Warning, TEXT(" - Material[%d]:x=%d, y=%d, z=%d, index=%f, RAW: %d"), i, p.X, p.Y, p.Z, Material.GetSingleIndex_AsFloat(), materialIndex);
							}
						}
						i++;
					}*/
				}
			}
			else
			{
				TargetVoxelWorld = nullptr;
			}

			if (hit->Implements<AVoxelWorldInterface>())
			{
				// UE_LOG(LogTemp, Warning, TEXT("Hit voxel world target %s (%d)"), *hit->GetName(), rand());
			}
			else
			{
				// UE_LOG(LogTemp, Warning, TEXT("Hit non-Interactable target %s (%d)"), *hit->GetName(), rand());
			}
			if (hits.Item != -1)
			{
				SetCurrentTarget(hit, hits.GetComponent(), hits.Item);
			}
			else
				SetCurrentTarget(nullptr);
		}
	}
	else
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), end, 5, 5, FLinearColor::Red);
		SetCurrentTarget(nullptr);
	}
}

void APrimitiveCharacter::SetCurrentTarget(AActor* target, UPrimitiveComponent* component, int32 instanceId)
{
	if (CurrentTarget != target || CurrentTargetInstanceId != instanceId || CurrentTargetComponent != component)
	{
		if (CurrentTarget != nullptr)
		{
			SetHighlightIfInteractableTarget(CurrentTarget, false);
			auto name = CurrentTarget->GetActorNameOrLabel();
			UE_LOG(LogTemp, Warning, TEXT("Target REMOVED %s"), *name);
		}

		if (target)
		{
			auto name = target->GetActorNameOrLabel();
			SetHighlightIfInteractableTarget(target, true);
			if (component && instanceId)
			{
				UE_LOG(LogTemp, Warning, TEXT("Target %s component %s instance %ld"), *name, *component->GetName(), instanceId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Target %s"), *name);
			}
		}
		CurrentTarget = target;
		CurrentTargetComponent = Cast<UInstancedStaticMeshComponent>(component);
		CurrentTargetInstanceId = instanceId;
		
		if (target != nullptr && UKismetSystemLibrary::DoesImplementInterface(target, UInteractable::StaticClass()))
			CurrentInteractable = Cast<AInteractableActor>(target);
		else
			CurrentInteractable = nullptr;

		if (CurrentInteractable)
			UE_LOG(LogTemp, Warning, TEXT("Target Interactable [%s]"), *CurrentInteractable->GetItem().Name);
	}
}

void APrimitiveCharacter::SetHighlightIfInteractableTarget(AActor* target, bool value)
{
	auto primos = target->GetComponentByClass<UPrimitiveComponent>();
	if (primos)
		primos->SetRenderCustomDepth(value);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APrimitiveCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Look);

		// Zooming
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ZoomIn);
		EnhancedInputComponent->BindAction(ZoomOutAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ZoomOut);

		EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::FreeLook);

		// Interactions
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Interact);
		EnhancedInputComponent->BindAction(PickAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Pick);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Drop);
		EnhancedInputComponent->BindAction(ConsumeAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Consume);
		EnhancedInputComponent->BindAction(CombineAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Combine);
		EnhancedInputComponent->BindAction(HitAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Hit);

		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Back);
		EnhancedInputComponent->BindAction(TransferAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Transfer);

		EnhancedInputComponent->BindAction(ShiftModifierDownAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ShiftModifierDown);
		EnhancedInputComponent->BindAction(ShiftModifierUpAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ShiftModifierUp);

		EnhancedInputComponent->BindAction(CtrlModifierDownAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::CtrlModifierDown);
		EnhancedInputComponent->BindAction(CtrlModifierUpAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::CtrlModifierUp);
	}

}

void APrimitiveCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APrimitiveCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void APrimitiveCharacter::ZoomIn(const FInputActionValue& Value)
{
	if (CurrentZoomLevel <= 1)
		CurrentZoomLevel = 0;
	else
		CurrentZoomLevel--;
	CameraBoom->TargetArmLength = ZoomTransforms[CurrentZoomLevel];
}

void APrimitiveCharacter::ZoomOut(const FInputActionValue& Value)
{
	if (CurrentZoomLevel >= ZoomTransforms.Num() - 2)
		CurrentZoomLevel = ZoomTransforms.Num() - 1;
	else
		CurrentZoomLevel++;
	CameraBoom->TargetArmLength = ZoomTransforms[CurrentZoomLevel];
}

void APrimitiveCharacter::FreeLook(const FInputActionValue& Value)
{
	// ???? TODO: ..or not to
}

void APrimitiveCharacter::Pick(const FInputActionValue& Value)
{
	if (CurrentInteractable != nullptr)
	{
		if (InventoryWidget != nullptr)
		{
			auto const& item = CurrentInteractable->GetItem();
			UE_LOG(LogTemp, Warning, TEXT("Adding item to inventory %s"), *item.Name);
			//item.Icon.LoadSynchronous(); // Force resolve lazy loading - not sure if this is the best way but seems to make it work. Maybe check icon.IsPending() to see the need to load
			if (Inventory->AddItem(item))
			{
				auto actor = CurrentInteractable;
				SetCurrentTarget(nullptr);
				actor->Destroy();
			}
		}
	}
}

void APrimitiveCharacter::Drop(const FInputActionValue& Value)
{
	if (Inventory && Inventory->CurrentSelectedSlotIndex != -1)
	{
		auto& slot = Inventory->GetSlotAt(Inventory->CurrentSelectedSlotIndex);
		auto n = slot.Count;
		if (ModifierShiftDown)
			n = 1;
		Inventory->DropItemsFromSlot(slot, n);
	}
}

void APrimitiveCharacter::Consume(const FInputActionValue& Value)
{
	if (CurrentInteractable)
	{
		IInteractable::Execute_Consume(CurrentInteractable);
	}
}

void APrimitiveCharacter::Combine(const FInputActionValue& Value)
{
	// ???? TODO:
}

void APrimitiveCharacter::Hit(const FInputActionValue& Value)
{
	if (CurrentInteractable)
	{
		IInteractable::Execute_Hit(CurrentInteractable);
	}
	else if (CurrentTarget && CurrentTargetComponent && CurrentTargetInstanceId != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit %s instance %ld"), *CurrentTargetComponent->GetName(), CurrentTargetInstanceId);
		auto fa = Cast<AInstancedFoliageActor>(CurrentTarget);
		auto co = Cast<UFoliageResource>(CurrentTargetComponent);
		if (fa && co)
		{
			HitFoliageInstance(*fa, *co, CurrentTargetInstanceId);
		}
	}
}

void
APrimitiveCharacter::Interact(const FInputActionValue& Value)
{
	if (CurrentInteractable)
	{
		IInteractable::Execute_Interact(CurrentInteractable);
	}
	else
	{
		if (TargetVoxelWorld)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Interact Voxels at [%f, %f, %f]"), TargetLocation.X, TargetLocation.Y, TargetLocation.Z);
			CollectMaterialsFrom(TargetLocation);
		}
		else if (CurrentTarget && CurrentTargetComponent && CurrentTargetInstanceId != -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interact %s instance %ld"), *CurrentTargetComponent->GetName(), CurrentTargetInstanceId);
			//auto fa = Cast<AInstancedFoliageActor>(CurrentTarget);
			// fa->SelectInstance(CurrentTargetComponent, CurrentTargetInstanceId, true);
		}
	}
}

void APrimitiveCharacter::ToggleInventory(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory %d"), Value.Get<bool>());

	auto pc = GetController<APlayerController>();
	if (ShowingInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory OFF"));
		ShowingInventory = false;
		if (InventoryWidget)
		{
			InventoryWidget->RemoveFromParent();
			if (pc)
			{
				FInputModeGameOnly mode;
				pc->SetInputMode(mode);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory ON"));
		ShowingInventory = true;
		if (InventoryWidget != nullptr)
		{
			InventoryWidget->AddToPlayerScreen();
			if (pc)
			{
				FInputModeGameAndUI mode;
				//auto w = TSharedPtr<UUserWidget>(InventoryWidget);
				//SWidget;
				//mode.SetWidgetToFocus(w);
				
				mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				mode.SetHideCursorDuringCapture(false);
				InventoryWidget->SetFocus();// SetUserFocus(pc);

				pc->SetInputMode(mode);
			}
		}
	}
}

void APrimitiveCharacter::Back(const FInputActionValue& Value)
{
	auto pc = GetController<APlayerController>();
	if (ShowingInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory OFF"));
		ShowingInventory = false;
		InventoryWidget->RemoveFromParent();
		if (pc)
		{
			FInputModeGameOnly mode;
			pc->SetInputMode(mode);
		}
	}
}

void APrimitiveCharacter::Transfer(const FInputActionValue& Value)
{
	// ???? TODO:
}

void
APrimitiveCharacter::HitFoliageInstance(AInstancedFoliageActor& inFoliageActor, UFoliageResource& inFoliageComponent, int32 inInstanceId)
{
	auto atrans = GetActorTransform();
	auto aloc = atrans.GetLocation();

	UE_LOG(LogTemp, Warning, TEXT("Hit %s foliage instance %ld"), *inFoliageComponent.GetName(), inInstanceId);

	FTransform trans;
	if (inFoliageComponent.GetInstanceTransform(inInstanceId, trans, true))
	{
		// ???? TODO: Perhaps move up a bit to avoid things going underground?
		for (auto& part : inFoliageComponent.BreaksIntoItems)
		{
			auto item = FindItem(part.ItemId);
			if (item)
			{
				for (int i = 0; i < part.Count; i++)
				{
					FActorSpawnParameters SpawnInfo;
					SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;// AdjustIfPossibleButAlwaysSpawn;

					trans.AddToTranslation(FVector(20.0f, 20.f, 20.0f));
					auto loc = trans.GetLocation();
					UE_LOG(LogTemp, Warning, TEXT("  - break foliage into %s at (%f, %f, %f)"), *item->ItemClass->GetName(), loc.X, loc.Y, loc.Z);
					auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(item->ItemClass, trans, SpawnInfo);
					itemActor->Item = *item;
					itemActor->Item.Quality = part.Quality;
				}
			}
		}
	}

	inFoliageComponent.RemoveInstance(inInstanceId);
	CurrentTargetInstanceId = -1;
	CurrentTargetComponent = nullptr;
	CurrentTarget = nullptr;
}

void
APrimitiveCharacter::EquipItem(UInventorySlot& FromSlot, UInventorySlot& ToSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Equip from slot % to slot %d"), FromSlot.SlotIndex, ToSlot.SlotIndex);
	// ???? TODO:
}

void
APrimitiveCharacter::UnequipItem(UInventorySlot& FromSlot, UInventorySlot& ToSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Uneqip from slot % to slot %d"), FromSlot.SlotIndex, ToSlot.SlotIndex);
	// ???? TODO:
}

void
APrimitiveCharacter::CreateDroppedItem(const FItemStruct& Item)
{
	FVector start = GetActorLocation();
	FVector end = start + FollowCamera->GetForwardVector().GetSafeNormal() * 50.0f;
	auto rotation = GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(Item.ItemClass, end, rotation, SpawnInfo);
}

TArray<ContainedMaterial>
APrimitiveCharacter::CollectMaterialsFrom(const FVector& Location)
{
	TArray<ContainedMaterial> collected;
	auto pos = TargetVoxelWorld->GlobalToLocal(Location);

	// UE_LOG(LogTemp, Warning, TEXT("Collecting Voxel at [%d, %d, %d]"), pos.X, pos.Y, pos.Z);

	auto& data = TargetVoxelWorld->GetData();
	TArray<FVoxelValueMaterial> voxels;
	TArray<FIntVector> poses;
	poses.Push(pos);
	UVoxelDataTools::GetVoxelsValueAndMaterial(voxels, TargetVoxelWorld, poses);
	for (auto& v : voxels)
	{
		auto& m = v.Material;
		if (v.Value >= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Collected Voxel value=%f, material={i0=%d, i1=%d, i2=%d, w=%d, b0=%d, b1=%d, b2=%d}, [%d, %d, %d]"), v.Value, m.GetMultiIndex_Index0(), m.GetMultiIndex_Index1(), m.GetMultiIndex_Index2(), m.GetMultiIndex_Blend0(), m.GetMultiIndex_Blend1(), m.GetMultiIndex_Blend2(), m.GetMultiIndex_Wetness(), pos.X, pos.Y, pos.Z);
		}
	}

	TArray<FModifiedVoxelValue> modified;
	UVoxelSphereTools::RemoveSphere(TargetVoxelWorld, Location, 50.0f, &modified);

	return collected;
}
