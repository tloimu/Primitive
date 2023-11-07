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
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "GameSettings.h"
#include "ItemDatabase.h"
#include "InstancedFoliageActor.h"
#include "Interactable.h"
#include "CrafterSlot.h"
#include "WorldGenOne.h"
#include "PrimitiveGameMode.h"
#include "OmaUtil.h"
#include "BuildingSnapBox.h"
#include "PrimitiveGameState.h"
#include "PrimitiveGameInstance.h"

#include <Voxel/Public/VoxelTools/Gen/VoxelSphereTools.h>
#include <Voxel/Public/VoxelWorldInterface.h>
#include <Voxel/Public/VoxelWorld.h>
#include <Voxel/Public/VoxelTools/Gen/VoxelToolsBase.h>
#include <Voxel/Public/VoxelTools/VoxelDataTools.h>


FName GAME_TAG_DOOR("Door");

//////////////////////////////////////////////////////////////////////////
// APrimitiveCharacter

APrimitiveCharacter::APrimitiveCharacter() : ACharacter()
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

	SetCharacterMovementNormal();

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->PostProcessSettings.AutoExposureMaxBrightness = 2.0f;
	FollowCamera->PostProcessSettings.AutoExposureMinBrightness = 0.7f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CurrentZoomLevel = 1;

	CurrentTarget = nullptr;
	CurrentTargetComponent = nullptr;
	CurrentTargetInstanceId = -1;
	ShowingInventory = false;
	TargetVoxelWorld = nullptr;

	InventoryWidgetClass = nullptr;
	InventoryWidget = nullptr;

	CrafterWidgetClass = nullptr;
	CrafterWidget = nullptr;

	HUDWidgetClass = nullptr;
	HUDWidget = nullptr;

	Inventory = nullptr;
	EquippedItems = nullptr;
}

void
APrimitiveCharacter::SetCharacterMovementNormal()
{
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void
APrimitiveCharacter::SetCharacterMovementSuperMoves()
{
	GetCharacterMovement()->JumpZVelocity = 10000.f;
	GetCharacterMovement()->MaxWalkSpeed = 10000.f;
	GetCharacterMovement()->MaxSwimSpeed = 10000.0f;
	GetCharacterMovement()->AirControl = 100.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 20000.f;
}


void APrimitiveCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentZoomLevel = 1;
	UpdateZoomSettings();

	Inventory = NewObject<UInventory>();
	Inventory->InventoryOwner = this;
	EquippedItems = NewObject<UInventory>();
	EquippedItems->InventoryOwner = this;
	HandCrafter = NewObject<UCrafter>();
	HandCrafter->Inventory = Inventory;
	HandCrafter->CrafterName = "Hand Crafting";

	for (auto& r : CraftableRecipies)
	{
		FCraftableItem ci;
		ci.CraftRecipieId = r;
		ci.Efficiency = 1.0f;
		ci.Quality = 1.0f;
		HandCrafter->CraftableItems.Add(ci);
	}

	auto pc = GetController<APlayerController>();
	if (pc)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		if (IsLocallyControlled())
		{
			SetupInventoryUI(pc);
			SetupCrafterUI(pc);
			SetupHUD(pc);
		}
	}

	EnsureNotUnderGround();

	if (MapWidgetClass)
	{
		MapWidget = CreateWidget<UMapWidget>(pc, MapWidgetClass);
		if (MapWidget)
		{
			MapWidget->SetVisibility(ESlateVisibility::Hidden);
			MapWidget->AddToPlayerScreen();
			ShowingMap = false;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create MapWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No MapWidget Class set"));
	}

	CheckBeginPlay();
}

void
APrimitiveCharacter::CheckBeginPlay()
{
	if (MapWidget)
	{
		MapWidget->GenerateMap();
	}
}

void
APrimitiveCharacter::SetupInventoryUI(APlayerController *pc)
{
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(pc, InventoryWidgetClass);
		check(InventoryWidget);
		InventoryWidget->Setup(Inventory, EquippedItems);
		Inventory->SetMaxSlots(24);
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		InventoryWidget->AddToPlayerScreen();
	}
}

void
APrimitiveCharacter::SetupHUD(APlayerController* pc)
{
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(pc, HUDWidgetClass);
		check(HUDWidget);
		HUDWidget->AddToPlayerScreen();
	}
}

void
APrimitiveCharacter::SetupCrafterUI(APlayerController* pc)
{
	UE_LOG(LogTemp, Warning, TEXT("Setup hand crafter ui"));
	ShowingHandCrafter = false;
	if (CrafterWidgetClass)
	{
		CrafterWidget = CreateWidget<UCrafterWidget>(pc, CrafterWidgetClass);
		check(CrafterWidget);
		CrafterWidget->SetCrafter(HandCrafter);
		CrafterWidget->SetVisibility(ESlateVisibility::Hidden);
		CrafterWidget->AddToPlayerScreen();
		ShowingCrafter = false;
	}
}

void
APrimitiveCharacter::EnsureNotUnderGround()
{
	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	if (WorldGenInstance)
	{
		if (WorldGenInstance->VoxelSize > 1.0f)
		{
			FIntVector l(GetActorLocation() / WorldGenInstance->VoxelSize);
			auto th = WorldGenInstance->GetTerrainHeight(l.X, l.Y, l.Z);
			FVector up = GetActorLocation();
			up.Z += FMath::Max(0.0f, th) * WorldGenInstance->VoxelSize + 2000.0f;
			UE_LOG(LogTemp, Warning, TEXT("Placed up %f cm"), up.Z);
			SetActorLocation(up);
		}
	}
}

const FItemStruct*
APrimitiveCharacter::FindItem(const FString& Id) const
{
	auto db = OmaUtil::GetItemDb(GetGameInstance());
	if (db)
		return db->FindItem(Id);
	else
		return nullptr;
}

void
APrimitiveCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearTimers();

	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}

	if (CrafterWidget)
	{
		CrafterWidget->RemoveFromParent();
		CrafterWidget = nullptr;
	}

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
}

void APrimitiveCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckTarget();

	CheckEnvironment();
	CheckCrafting(DeltaSeconds);
	CheckCurrentPlacedItem();
}

void
APrimitiveCharacter::CheckEnvironment()
{
	if (!StartingPlaceSet)
	{
		EnsureNotUnderGround();
		StartingPlaceSet = true;
	}

	auto WorldGenInstance = FWorldGenOneInstance::sGeneratorInstance;
	if (WorldGenInstance)
	{
		if (WorldGenInstance->VoxelSize > 1.0f) // ???? Optimize: Easier access to VoxelSize
		{
			FIntVector l(GetActorLocation() / WorldGenInstance->VoxelSize);
			auto th = WorldGenInstance->GetTerrainHeight(l.X, l.Y, l.Z);
			if (l.Z < th - 10.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Jumped UP %f"), WorldGenInstance->VoxelSize);
				FVector up = GetActorLocation();
				up.Z += FMath::Max(0.0f, th) * WorldGenInstance->VoxelSize + 500.0f;
				SetActorLocation(up);
			}

			auto T = WorldGenInstance->GetTemperature(l.X, l.Y, l.Z);
			auto M = WorldGenInstance->GetMoisture(l.X, l.Y, l.Z);
			if (HUDWidget)
			{
				HUDWidget->SetEnvironment(T, M);
				float lat = WorldGenInstance->GetLatitude(l.Y);
				HUDWidget->SetLocation(l, lat);
				HUDWidget->SetTerrainHeight(th);
				HUDWidget->SetHealth(99.0f);
				HUDWidget->SetStamina(92.0f);
				auto gs = Cast<APrimitiveGameState>(GetWorld()->GetGameState());
				if (gs)
				{
					HUDWidget->SetClock(gs->ClockInSecs);
				}
			}
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
APrimitiveCharacter::CheckCrafting(float DeltaSeconds)
{
	if (HandCrafter)
	{
		auto gs = Cast<APrimitiveGameState>(GetWorld()->GetGameState());
		if (gs)
		{
			HandCrafter->CheckCrafting(DeltaSeconds * gs->ClockSpeed); // ???? TODO: Optimize getting ClockSpeed
		}
	}
}

void
APrimitiveCharacter::CheckTarget()
{
	FHitResult hits;
	ECollisionChannel channel = ECollisionChannel::ECC_Visibility;
	FVector start = FollowCamera->GetComponentTransform().GetLocation();
	FVector end = start + FollowCamera->GetForwardVector().GetSafeNormal() * (CameraBoom->TargetArmLength + 500.0f);

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (CurrentPlacedItem)
		params.AddIgnoredActor(CurrentPlacedItem);
	params.bTraceComplex = true;
	auto ok = GetWorld()->LineTraceSingleByChannel(hits, start, end, channel, params);

	if (CurrentPlacedItem && (CurrentPlacedItem->RequireSnapBox || ModifierCtrlDown))
	{
		
		FHitResult snapHits;
		channel = ECollisionChannel::ECC_GameTraceChannel1;
		FVector endPlace = start + FollowCamera->GetForwardVector().GetSafeNormal() * 1000.0f;
		GetWorld()->LineTraceSingleByChannel(snapHits, start, endPlace, channel, params);
		if (snapHits.bBlockingHit)
		{
			if (snapHits.GetComponent())
			{
				auto buildSnapBox = Cast<UBuildingSnapBox>(snapHits.GetComponent());
				bool allow = AllowPlaceItem(*CurrentPlacedItem, buildSnapBox);
				SetHighlightIfInteractableTarget(CurrentPlacedItem, allow);
				if (buildSnapBox && allow)
				{
					CurrentBuildSnapBox = buildSnapBox;
					auto cl = buildSnapBox->GetComponentLocation();
					auto cr = buildSnapBox->GetComponentRotation();
					TargetLocation = cl;
					OmaUtil::TeleportActor(*CurrentPlacedItem, cl, cr);
					return;
				}
				else
				{
					CurrentBuildSnapBox = nullptr;
					TargetLocation = hits.Location;
					auto rot = CurrentPlacedItem->GetActorRotation();
					OmaUtil::TeleportActor(*CurrentPlacedItem, TargetLocation, rot);
					return;
				}
			}
		}
	}


	if (hits.bBlockingHit)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, hits.Location, FColor(100, 0, 0));
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), hits.Location, 5, 5, FLinearColor::White);

		auto hit = hits.GetActor();

		if (hit != CurrentPlacedItem)
			TargetLocation = hits.Location;

		if (hit && hit->Implements<UInteractable>())
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
		TargetLocation = end;
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), end, 5, 5, FLinearColor::Red);
		SetCurrentTarget(nullptr);
	}
}

void APrimitiveCharacter::SetCurrentTarget(AActor* target, UPrimitiveComponent* component, int32 instanceId)
{
	if (CurrentPlacedItem)
	{
		return;
	}

	if (CurrentTarget != target || CurrentTargetInstanceId != instanceId || CurrentTargetComponent != component)
	{
		if (CurrentTarget != nullptr)
		{
			SetHighlightIfInteractableTarget(CurrentTarget, false);
			auto name = CurrentTarget->GetActorNameOrLabel();
			UE_LOG(LogTemp, Warning, TEXT("Target REMOVED %s"), *name);
		}

		CurrentTarget = target;
		CurrentTargetComponent = Cast<UInstancedStaticMeshComponent>(component);
		CurrentTargetInstanceId = instanceId;

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
		
		if (target != nullptr && UKismetSystemLibrary::DoesImplementInterface(target, UInteractable::StaticClass()))
			CurrentInteractable = Cast<AInteractableActor>(target);
		else
			CurrentInteractable = nullptr;

		if (CurrentInteractable)
		{
			auto loc = CurrentInteractable->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Target Interactable [%s] at [%f, %f, %f]"), *CurrentInteractable->GetItem().Name, loc.X, loc.Y, loc.Z);
		}
	}
}

void APrimitiveCharacter::SetHighlightIfInteractableTarget(AActor* target, bool value)
{
	auto ia = Cast<AInteractableActor>(target);
	if (ia)
	{
		/*
		auto primos = target->GetComponentByClass<UPrimitiveComponent>();
		if (primos)
			primos->SetRenderCustomDepth(value);
		*/
		auto primos = target->GetComponents();
		for (auto p : primos)
		{
			auto c = Cast<UPrimitiveComponent>(p);
			if (c)
				c->SetRenderCustomDepth(value);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void
APrimitiveCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(ToggleCrafterAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ToggleCrafter);
		EnhancedInputComponent->BindAction(ToggleMapAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ToggleMap);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Back);
		EnhancedInputComponent->BindAction(TransferAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::Transfer);

		EnhancedInputComponent->BindAction(ShiftModifierDownAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ShiftModifierDown);
		EnhancedInputComponent->BindAction(ShiftModifierUpAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::ShiftModifierUp);

		EnhancedInputComponent->BindAction(CtrlModifierDownAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::CtrlModifierDown);
		EnhancedInputComponent->BindAction(CtrlModifierUpAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::CtrlModifierUp);

		EnhancedInputComponent->BindAction(QuickSaveGameAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::QuickSaveGame);
		EnhancedInputComponent->BindAction(QuickLoadGameAction, ETriggerEvent::Triggered, this, &APrimitiveCharacter::QuickLoadGame);
	}

}

// ---------------------------------------------
// Sounds

void
APrimitiveCharacter::PlaySound(USoundCue* inDefaultSound, USoundCue* inOverrideSound) const
{
	auto sound = inOverrideSound ? inOverrideSound : inDefaultSound;
	if (sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), sound);
	}
}


void
APrimitiveCharacter::PlaySoundCrafting(const FItemStruct& inItem) const
{
	PlaySound(HandCraftingSound);
}

void
APrimitiveCharacter::PlaySoundHit(const FItemStruct* inItem, const UHISMFoliage* inResource) const
{
	PlaySound(HitItemSound);
}

void
APrimitiveCharacter::PlaySoundEquip(const FItemStruct& inItem) const
{
	PlaySound(EquipItemSound);
}

void
APrimitiveCharacter::PlaySoundUnequip(const FItemStruct& inItem) const
{
	PlaySound(UnequipItemSound);
}

void
APrimitiveCharacter::PlaySoundDropItem(const FItemStruct& inItem) const
{
	PlaySound(DropItemSound);
}

void
APrimitiveCharacter::PlaySoundPickItem(const FItemStruct& inItem) const
{
	PlaySound(PickItemSound);
}

void
APrimitiveCharacter::PlaySoundOpenDoor(const FItemStruct& inItem) const
{
	PlaySound(OpenWoodDoorSound);
}

void
APrimitiveCharacter::PlaySoundCloseDoor(const FItemStruct& inItem) const
{
	PlaySound(CloseWoodDoorSound);
}

void
APrimitiveCharacter::PlaySoundHarvest() const
{
	PlaySound(HarvestSound);
}

void
APrimitiveCharacter::PlaySoundToggleInventory(bool OpenInventory) const
{
	PlaySound(ToggleInventorySound);
}


// Sounds
// ---------------------------------------------


void
APrimitiveCharacter::Move(const FInputActionValue& Value)
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
	if (CurrentPlacedItem && CurrentPlacedItem->IsFoundation)
	{
		if (ModifierCtrlDown)
		{
			CurrentPlacedItemElevation += PlacedItemElevationStep;
		}
		else if (ModifierShiftDown)
		{
			CurrentPlacedItemRotation -= PlacedItemRotationStep;
		}
	}
	else
	{
		UpdateZoomSettings(-1);
	}
}

void APrimitiveCharacter::ZoomOut(const FInputActionValue& Value)
{
	if (CurrentPlacedItem && CurrentPlacedItem->IsFoundation)
	{
		if (ModifierCtrlDown)
		{
			CurrentPlacedItemElevation -= PlacedItemElevationStep;
		}
		else if (ModifierShiftDown)
		{
			CurrentPlacedItemRotation += PlacedItemRotationStep;
		}
	}
	else
	{
		UpdateZoomSettings(1);
	}
}

void
APrimitiveCharacter::UpdateZoomSettings(int Adjust)
{
	CurrentZoomLevel += Adjust;

	if (CurrentZoomLevel < 0)
		CurrentZoomLevel = 0;

	if (CurrentZoomLevel >= ZoomTransforms.Num())
		CurrentZoomLevel = ZoomTransforms.Num() - 1;

	auto& z = ZoomTransforms[CurrentZoomLevel];

	CameraBoom->TargetArmLength = z.ArmLength;
	CameraBoom->SocketOffset = z.Offset;
	FollowCamera->SetFieldOfView(z.Fov);
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
			if (CurrentInteractable->CanBePicked())
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding item to inventory %s"), *item.Name);
				if (Inventory->AddItem(item))
				{
					PlaySoundPickItem(item);
					if (CurrentInteractable->Inventory)
					{
						// Move container items into inventory if possible
						for (auto& i : CurrentInteractable->Inventory->Slots)
						{
							if (i.Count > 0)
							{
								if (!Inventory->AddItem(i.Item, i.Count))
									return; // cannot fit everything, stop here
									return; // cannot fit everything, stop here
							}
							i.Count = 0;
						}
					}
					auto actor = CurrentInteractable;
					SetCurrentTarget(nullptr);
					actor->DestroyItem();
				}
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
		PlaySoundDropItem(slot.Item);
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
	if (CurrentPlacedItem)
	{
		CompletePlacingItem();
		return;
	}

	if (CurrentInteractable)
	{
		IInteractable::Execute_Hit(CurrentInteractable);
	}
	else if (CurrentTarget && CurrentTargetComponent && CurrentTargetInstanceId != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit %s instance %ld"), *CurrentTargetComponent->GetName(), CurrentTargetInstanceId);
		if (!CommittedToAction)
			CommitToHitAction();
	}
}

void
APrimitiveCharacter::ClearTimers()
{
	GetWorldTimerManager().ClearTimer(CommittedActionTimerHandle);
}

void
APrimitiveCharacter::CommitToHitAction()
{
	CommittedToAction = true;
	auto HitActionDuration = 1.5f;
	auto hfa = Cast<AHISMFoliageActor>(CurrentTarget);
	auto fa = Cast<AActor>(CurrentTarget);
	auto co = Cast<UHISMFoliage>(CurrentTargetComponent);
	int32 i = CurrentTargetInstanceId;
	if (hfa && fa && co)
	{
		UE_LOG(LogTemp, Warning, TEXT("Commit to Hit %s instance %ld"), *CurrentTargetComponent->GetName(), CurrentTargetInstanceId);
		PlaySoundHit(nullptr, co);
		FTimerDelegate caller = FTimerDelegate::CreateUObject(this, &APrimitiveCharacter::HitExecute, fa, co, i);
		GetWorldTimerManager().SetTimer(CommittedActionTimerHandle, caller, HitActionDuration, false);
	}
}

void
APrimitiveCharacter::HitExecute(AActor* inFoliageActor, UHISMFoliage *inResourceComponent, int32 inInstanceId)
{
	CommittedToAction = false;
	HitFoliageInstance(*inFoliageActor, *inResourceComponent, inInstanceId);
}


void
APrimitiveCharacter::Interact(const FInputActionValue& Value)
{
	if (CurrentPlacedItem)
	{
		CancelPlaceItem();
	}
	else if (ShowingInventory)
	{
		Interact_InInventory(Value);
	}
	else
	{
		if (CurrentInteractable)
		{
			Interact_Interactable(Value, *CurrentInteractable);
		}
		else
		{
			if (TargetVoxelWorld)
			{
				Interact_Voxel(Value, TargetLocation);
			}
			else if (CurrentTarget && CurrentTargetComponent && CurrentTargetInstanceId != -1)
			{
				Interact_Foliage(Value, *CurrentTargetComponent, CurrentTargetInstanceId);
			}
		}
	}
}

void
APrimitiveCharacter::Interact_InInventory(const FInputActionValue& Value)
{
	auto slotIndex = Inventory ? Inventory->CurrentSelectedSlotIndex : -1;
	UE_LOG(LogTemp, Warning, TEXT("Interact Inventory Slot %d"), slotIndex);
	if (slotIndex >= 0)
	{
		auto& slot = Inventory->GetSlotAt(slotIndex);
		if (slot.Count > 0)
		{
			// ???? TODO: Consider consumables - need a ItemStruct info for it
			if (slot.Item.IsConsumable())
				ConsumeItem(slot);
			if (!slot.Item.CanWearIn.IsEmpty())
				WearItem(slot);
			else
			{
				ToggleInventoryUI();
				StartPlacingItem(slot);
			}
		}
	}
}

void
APrimitiveCharacter::Interact_Voxel(const FInputActionValue& Value, const FVector& inTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("Interact Voxels at [%f, %f, %f]"), inTarget.X, inTarget.Y, inTarget.Z);
	CollectMaterialsFrom(inTarget);
}

void
APrimitiveCharacter::Interact_Interactable(const FInputActionValue& Value, AInteractableActor &inTarget)
{
	if (IInteractable::Execute_Interact(&inTarget))
		return;

	if (inTarget.Inventory)
	{
		if (InventoryWidget)
			InventoryWidget->SetContainer(&inTarget);
		ToggleInventoryUI();
	}
	else
	{
		// ???? TODO: Add some "pickable" attribute to item specs as not everything can be interacted with can be picked?
		if (CurrentInteractable->ActorHasTag(GAME_TAG_DOOR))
		{
			InteractDoor(*CurrentInteractable);
		}
		else if (CurrentInteractable->Item.UsableFor.Contains(EItemUtility::Building))
		{
			// no picking up of building components
		}
		else
			Pick(Value);
	}
}

void
APrimitiveCharacter::Interact_Foliage(const FInputActionValue& Value, UInstancedStaticMeshComponent& inFoliageComponent, int32 inInstanceId)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact %s instance %ld"), *inFoliageComponent.GetName(), inInstanceId);
	//auto fa = Cast<AInstancedFoliageActor>(CurrentTarget);
	// fa->SelectInstance(CurrentTargetComponent, CurrentTargetInstanceId, true);
}


void
APrimitiveCharacter::InteractDoor(AInteractableActor& inDoor)
{
	FVector o; FVector box;
	inDoor.GetActorBounds(true, o, box, false);

	// ???? TODO: Make the door open/close slowly instead of teleporting
	if (inDoor.CurrentState == 0)
	{
		PlaySoundOpenDoor(inDoor.Item);
		inDoor.OriginalStateTransform = inDoor.GetActorTransform();

		auto a = box.X;
		auto b = box.X / 1.4;
		FVector d((a + b), -b, 0);
		FRotator rot(0, 135, 0);
		//OmaUtil::RotateActorAroundPoint(inDoor, inDoor.GetActorLocation(), rot);
		inDoor.AddActorWorldRotation(FRotator(0, 135, 0));
		inDoor.AddActorWorldOffset(d);
		inDoor.CurrentState = 1;
	}
	else
	{
		PlaySoundCloseDoor(inDoor.Item);
		inDoor.SetActorTransform(inDoor.OriginalStateTransform);
		inDoor.CurrentState = 0;
	}
}

void
APrimitiveCharacter::WearItem(FItemSlot& fromSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Wear %s from inventory slot %d"), *fromSlot.Item.Name, fromSlot.Index);
	PlaySoundEquip(fromSlot.Item);
	for (auto& eqSlot : EquippedItems->Slots)
	{
		if (fromSlot.CanMergeTo(eqSlot))
		{
			if (fromSlot.MergeTo(eqSlot, 1))
				return;
		}
	}
}

void
APrimitiveCharacter::ConsumeItem(FItemSlot& fromSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Consume %s from inventory slot %d"), *fromSlot.Item.Name, fromSlot.Index);
	if (fromSlot.Item.ConsumedWater > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gained %d water"), fromSlot.Item.ConsumedWater);
	}
	if (fromSlot.Item.ConsumedFood > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gained %d food"), fromSlot.Item.ConsumedFood);
	}
	if (fromSlot.Item.ConsumedAir > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gained %d air"), fromSlot.Item.ConsumedAir);
	}
	fromSlot.ChangeCountBy(-1).NotifyChange();
	// ???? TODO: Alter character stats?
}


void
APrimitiveCharacter::ToggleInventory(const FInputActionValue& Value)
{
	ToggleInventoryUI();
}

void
APrimitiveCharacter::ToggleCrafter(const FInputActionValue& Value)
{
	ToggleCrafterUI();
}

void
APrimitiveCharacter::ToggleMap(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Map"));
	if (ShowingMap)
	{
		ShowingMap = false;
		if (MapWidget)
			MapWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ShowingMap = true;
		if (MapWidget)
			MapWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void
APrimitiveCharacter::ToggleInventoryUI()
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory"));

	auto pc = GetController<APlayerController>();
	if (ShowingInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory OFF"));
		ShowingInventory = false;
		PlaySoundToggleInventory(false);
		if (InventoryWidget)
		{
			InventoryWidget->SetContainer(nullptr);
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
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
		if (ShowingCrafter)
			ToggleCrafterUI();
		ShowingInventory = true;
		PlaySoundToggleInventory(true);
		if (CurrentPlacedItem)
		{
			CancelPlaceItem();
		}
		if (InventoryWidget != nullptr)
		{
			if (CurrentInteractable && CurrentInteractable->Inventory)
			{
				InventoryWidget->SetContainer(CurrentInteractable);
			}
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
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


void
APrimitiveCharacter::ToggleCrafterUI()
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Crafter"));

	auto pc = GetController<APlayerController>();
	if (ShowingCrafter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Crafter OFF"));
		ShowingCrafter = false;
		if (CrafterWidget)
		{
			CrafterWidget->SetCrafter(nullptr);
			CrafterWidget->SetVisibility(ESlateVisibility::Hidden);
			if (pc)
			{
				FInputModeGameOnly mode;
				pc->SetInputMode(mode);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Crafter ON"));
		if (ShowingInventory)
			ToggleInventoryUI();
		ShowingCrafter = true;
		if (CurrentPlacedItem)
		{
			CancelPlaceItem();
		}
		if (CrafterWidget != nullptr)
		{
			if (CurrentInteractable && CurrentInteractable->Crafter)
			{
				UE_LOG(LogTemp, Warning, TEXT("Setting Target Item Crafter"));
				CrafterWidget->SetCrafter(CurrentInteractable->Crafter);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Setting Hand Crafter"));
				CrafterWidget->SetCrafter(HandCrafter);
			}
			CrafterWidget->SetVisibility(ESlateVisibility::Visible);
			if (pc)
			{
				FInputModeGameAndUI mode;

				mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				mode.SetHideCursorDuringCapture(false);
				CrafterWidget->SetFocus();// SetUserFocus(pc);

				pc->SetInputMode(mode);
			}
		}
	}
}

void APrimitiveCharacter::Back(const FInputActionValue& Value)
{
	if (ShowingInventory)
	{
		ToggleInventoryUI();
	}
	if (ShowingCrafter)
	{
		ToggleCrafterUI();
	}
}

void
APrimitiveCharacter::Transfer(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Transfer"));

	if (ShowingInventory)
	{
		if (Inventory && CurrentInteractable && CurrentInteractable->Inventory)
		{
			// Transfer items back and forth between player and container inventories
			auto playerSlot = Inventory->GetSelectedSlot();
			auto containerSlot = CurrentInteractable->Inventory->GetSelectedSlot();
			auto containerInventory = CurrentInteractable->Inventory;
			if (playerSlot)
			{
				UE_LOG(LogTemp, Warning, TEXT("Transfer from slot %d to container"), playerSlot->Index);
				containerInventory->MoveItemsFrom(*playerSlot, playerSlot->Count);
			}
			else if (containerSlot)
			{
				UE_LOG(LogTemp, Warning, TEXT("Transfer from container slot %d to player"), containerSlot->Index);
				Inventory->MoveItemsFrom(*containerSlot, containerSlot->Count);
			}
		}
	}
	else
	{
		if (CurrentInteractable)
		{
			if (CurrentInteractable->Inventory)
			{
				if (InventoryWidget)
					InventoryWidget->SetContainer(CurrentInteractable);
				ToggleInventoryUI();
			}
		}
	}
}


void
APrimitiveCharacter::ShiftModifierDown(const FInputActionValue& Value)
{
	ModifierShiftDown = true;
	SetCharacterMovementSuperMoves();
}

void
APrimitiveCharacter::ShiftModifierUp(const FInputActionValue& Value)
{
	ModifierShiftDown = false;

	SetCharacterMovementNormal();
}

void
APrimitiveCharacter::CtrlModifierDown(const FInputActionValue& Value)
{
	ModifierCtrlDown = true;
}

void
APrimitiveCharacter::CtrlModifierUp(const FInputActionValue& Value)
{
	ModifierCtrlDown = false;
}

void
APrimitiveCharacter::QuickSaveGame(const FInputActionValue& Value)
{
	auto gi = Cast<UPrimitiveGameInstance>(GetGameInstance());
	if (gi)
	{
		if (ModifierCtrlDown)
			gi->SaveGame(GetDefaultSaveName());
		else
			gi->SaveGame(GetQuickSaveName());
	}
}

void
APrimitiveCharacter::QuickLoadGame(const FInputActionValue& Value)
{
	auto gi = Cast<UPrimitiveGameInstance>(GetGameInstance());
	if (gi)
	{
		if (ModifierCtrlDown)
			gi->ResetWorldToSavedGame(GetDefaultSaveName());
		else
			gi->ResetWorldToSavedGame(GetQuickSaveName());
	}
}

FString
APrimitiveCharacter::GetDefaultSaveName() const
{
	return DefaultSaveGameName + GetWorld()->GetName();
}

FString
APrimitiveCharacter::GetQuickSaveName() const
{
	return QuickSaveGameName + GetWorld()->GetName();
}


void
APrimitiveCharacter::HitFoliageInstance(AActor& inFoliageActor, UHISMFoliage& inFoliageComponent, int32 inInstanceId)
{
	auto atrans = GetActorTransform();
	auto aloc = atrans.GetLocation();

	UE_LOG(LogTemp, Warning, TEXT("Hit %s foliage instance %ld"), *inFoliageComponent.GetName(), inInstanceId);

	FTransform trans;
	if (inFoliageComponent.GetInstanceTransform(inInstanceId, trans, true))
	{
		// ???? TODO: Perhaps move up a bit to avoid things going underground?
		PlaySoundHarvest();
		for (auto& part : inFoliageComponent.BreaksIntoItems)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - break foliage into %d of %s"), part.Count, *part.ItemId);
			auto item = FindItem(part.ItemId);
			if (item)
			{
				for (int i = 0; i < part.Count; i++)
				{
					trans.AddToTranslation(FVector(0.0f, 0.0f, 35.0f)); // Some staggering - ???? TODO: Replace with transformation in FoliageResource
					auto loc = trans.GetLocation();
					auto rot = trans.GetRotation().Rotator();
					UE_LOG(LogTemp, Warning, TEXT("  - break foliage into %s at (%f, %f, %f)"), *item->ItemClass->GetName(), loc.X, loc.Y, loc.Z);
					auto itemActor = SpawnItem(*item, loc, rot);
					check(itemActor);
					itemActor->Item.Quality = part.Quality;
				}
				trans.AddToTranslation(FVector(0, 200.0f, -part.Count * 35.0f)); // Some staggering - ???? TODO: Replace with transformation in FoliageResource
			}
		}
	}

	inFoliageComponent.RemoveInstance(inInstanceId);
	SetCurrentTarget(nullptr);
}


AInteractableActor*
APrimitiveCharacter::DropItem(const FItemStruct& Item)
{
	FVector location, throwTo;
	FRotator rotation;
	GetItemDropPosition(location, rotation, throwTo);
	return SpawnItem(Item, location, rotation);
}

// ----------------------------
// Placing Item
// ----------------------------

void
APrimitiveCharacter::GetItemDropPosition(FVector& outLocation, FRotator& outRotation, FVector& outThrowTowards) const
{
	FVector start = GetActorLocation();
	outLocation = start + FollowCamera->GetForwardVector().GetSafeNormal() * 50.0f;
	outRotation = GetActorRotation();
	outThrowTowards = FVector();
}

AInteractableActor*
APrimitiveCharacter::SpawnItem(const FItemStruct& Item, const FVector& inLocation, const FRotator& inRotation)
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
			inv->InventoryOwner = this;
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
				crafter->Inventory = Inventory; // ???? CHECK: Memory managements - maybe refactor so the player inventory is accessed only when needed
			for (auto &rid : Item.CraftableRecipies)
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
APrimitiveCharacter::StartPlacingItem(FItemSlot& fromSlot)
{
	if (CurrentPlacedItem == nullptr && fromSlot.Count > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Placing %s from inventory slot %d"), *fromSlot.Item.Name, fromSlot.Index);

		FVector start = GetActorLocation();
		FVector location = start + FollowCamera->GetForwardVector().GetSafeNormal() * MaxPlaceItemDistance;
		auto rotation = GetActorRotation();
		rotation.Yaw = FMath::Floor(rotation.Yaw / PlacedItemRotationStep) * PlacedItemElevationStep; // Align with the world coordinates

		CurrentPlacedItem = SpawnItem(fromSlot.Item, location, rotation);
		if (CurrentPlacedItem)
		{
			CurrentPlacedItemFromSlot = &fromSlot;
			CurrentPlacedItemElevation = 0;
			CurrentPlacedItemRotation = 0;
			OmaUtil::DisablePhysicsAndCollision(*CurrentPlacedItem);
			SetHighlightIfInteractableTarget(CurrentPlacedItem, true);
		}
	}
}

void
APrimitiveCharacter::CancelPlaceItem()
{
	if (CurrentPlacedItem)
	{
		auto item = CurrentPlacedItem;
		CurrentPlacedItem = nullptr;
		CurrentPlacedItemFromSlot = nullptr;
		CurrentBuildSnapBox = nullptr;
		item->Destroy();
	}
}

bool
APrimitiveCharacter::CheckCurrentPlacedItem()
{
	if (CurrentPlacedItem)
	{
		FVector pos = TargetLocation;
		pos.Z = pos.Z + CurrentPlacedItemElevation;
		auto rot = CurrentPlacedItem->GetActorRotation();
		rot.Yaw += CurrentPlacedItemRotation;
		CurrentPlacedItemRotation = 0; // apply rotation only once

		auto ok = OmaUtil::TeleportActor(*CurrentPlacedItem, pos, rot);
		return ok;
	}
	else
		return false;
}

bool
APrimitiveCharacter::AllowPlaceItem(AInteractableActor& inItem, UBuildingSnapBox* inSnapBox) const
{
	if (inItem.RequireFoundation)
	{
		if (inSnapBox)
			return inSnapBox->PlaceStacksUp;
		else
			return false;
	}

	if (inItem.IsFoundation)
	{
		if (inSnapBox && inSnapBox->PlaceStacksUp)
			return false;
	}

	if (inItem.RequireSnapBox)
		return (inSnapBox != nullptr);

	return true;
}



void
APrimitiveCharacter::CompletePlacingItem()
{
	if (CurrentPlacedItem)
	{
		auto loc = CurrentPlacedItem->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Completed placing item %s to [%f, %f, %f]"), *CurrentPlacedItem->GetName(), loc.X, loc.Y, loc.Z);
		if (AllowPlaceItem(*CurrentPlacedItem, CurrentBuildSnapBox))
		{
			FinalizePlacingItem();
		}
		else
		{
			CancelPlaceItem();
		}
	}
}

void
APrimitiveCharacter::FinalizePlacingItem()
{
	if (CurrentBuildSnapBox)
	{
		if (CurrentBuildSnapBox->PlaceStacksUp)
		{
			auto support = Cast<AInteractableActor>(CurrentBuildSnapBox->GetOwner());
			if (support)
			{
				CurrentPlacedItem->AddOnItem(*support);
				UE_LOG(LogTemp, Warning, TEXT("  - supported by %s"), *support->GetName());
			}
		}
	}
	OmaUtil::EnableCollision(*CurrentPlacedItem);
	SetHighlightIfInteractableTarget(CurrentPlacedItem, false);
	if (CurrentPlacedItemFromSlot)
	{
		CurrentPlacedItemFromSlot->ChangeCountBy(-1).NotifyChange();
	}
	auto itemId = CurrentPlacedItem->Item.Id;
	CurrentPlacedItem = nullptr;
	CurrentPlacedItemFromSlot = nullptr;
	CurrentBuildSnapBox = nullptr;
	StartPlacingNextItemIfPossible(itemId);
}

void
APrimitiveCharacter::StartPlacingNextItemIfPossible(const FString& Id)
{
	if (Inventory)
	{
		auto slot = Inventory->FindFirstSlotOf(Id);
		if (slot)
			StartPlacingItem(*slot);
	}
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
	UVoxelSphereTools::RemoveSphere(TargetVoxelWorld, Location, 20.0f, &modified);

	return collected;
}


// ---------------------------------------------------------------------------
// Console commands
// ---------------------------------------------------------------------------

void
APrimitiveCharacter::priadditem(const FString& Id)
{
	CheatAddItems(Id, 1);
}

void
APrimitiveCharacter::priadditems(const FString& Id, int Count)
{
	CheatAddItems(Id, Count);
}

void
APrimitiveCharacter::priresetmap()
{
	auto gi = Cast<UPrimitiveGameInstance>(GetGameInstance());
	if (gi)
	{
		CheatDestroyAll(false, true);
		gi->GenerateWorld();
		gi->GenerateFoilage();
	}
}
void
APrimitiveCharacter::prihelp()
{
	UE_LOG(LogTemp, Warning, TEXT("pridestroyallitems - Destroys all items in the map"));
	UE_LOG(LogTemp, Warning, TEXT("priadditem <ItemId> - Add one item to inventory if possible"));
	UE_LOG(LogTemp, Warning, TEXT("priadditems <ItemId> <Count> - Add given count of items to inventory if possible"));
	UE_LOG(LogTemp, Warning, TEXT("priresetmap - Resets the terrain and foliage"));
	UE_LOG(LogTemp, Warning, TEXT("pridestroyallitems - Delete all items"));
	UE_LOG(LogTemp, Warning, TEXT("pridestroyallresources - Delete all foliage"));
	UE_LOG(LogTemp, Warning, TEXT("pridestroyall - Delete all items and foliage"));
	UE_LOG(LogTemp, Warning, TEXT("primoveup <Meters> - Move character given meters up"));
	UE_LOG(LogTemp, Warning, TEXT("prisethour <int> - Set the hour of the day"));
}

void
APrimitiveCharacter::pridestroyallitems()
{
	CheatDestroyAll(true, false);
}

void
APrimitiveCharacter::pridestroyallresources()
{
	CheatDestroyAll(false, true);
}

void
APrimitiveCharacter::pridestroyall()
{
	CheatDestroyAll(true, true);
}

void
APrimitiveCharacter::primoveup(int meters)
{
	FVector o;
	o.Z = meters * 100.0f;
	AddActorWorldOffset(o);
}


void
APrimitiveCharacter::CheatDestroyAll(bool inItems, bool inResources)
{
	auto gi = GetGameInstance<UPrimitiveGameInstance>();
	if (gi)
	{
		gi->DestroyAllItemsAndResources(inItems, inResources);
	}
}

void
APrimitiveCharacter::CheatAddItems(const FString &Id, int Count)
{
	UE_LOG(LogTemp, Warning, TEXT("Adding %d of %s items to player inventory"), Count, *Id);
	auto item = FindItem(Id);
	if (item)
	{
		if (Inventory)
		{
			if (Inventory->AddItem(*item, Count))
			{
				UE_LOG(LogTemp, Warning, TEXT("Added %d of %s items to player inventory"), Count, *item->Id);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to add item to player inventory"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No player inventory"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown item: %s"), *Id);
	}
}
