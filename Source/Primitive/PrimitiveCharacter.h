// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Runtime/Engine/Classes/Sound/SoundCue.h>
#include "InputActionValue.h"
#include "InventoryWidget.h"
#include "CrafterWidget.h"
#include "HUDWidget.h"
#include "GameSettings.h"
#include "Inventory.h"
#include "Craftable.h"
#include "MapWidget.h"
#include "FoliageResource.h"
#include "PrimitiveCharacter.generated.h"

struct ContainedMaterial
{
	int32 index;
	float amount;
};

class FWorldGenOneInstance;

enum FActionType
{
	OpenDoor,
	CloseDoor
};

class FAction
{
	FActionType	Action;
	AInteractableActor* Target;
};

UCLASS(config=Game)
class APrimitiveCharacter : public ACharacter, public IInventoryOwner
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Zoom Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FreeLookAction;

	// Character actions

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HitAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ConsumeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CombineAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleCrafterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TransferAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShiftModifierDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CtrlModifierDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShiftModifierUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CtrlModifierUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* QuickSaveGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* QuickLoadGameAction;

public:
	APrimitiveCharacter();
	
	UPROPERTY(EditAnywhere) USoundCue* HandCraftingSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* HitItemSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* PickItemSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* DropItemSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* EquipItemSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* UnequipItemSound = nullptr;
	UPROPERTY(EditAnywhere) USoundCue* HarvestSound = nullptr;

	// Sounds
	void PlaySound(USoundCue *inDefaultSound, USoundCue *inOverrideSound = nullptr) const;
	void PlaySoundCrafting(const FItemStruct& inItem) const override;
	void PlaySoundHit(const FItemStruct *inItem, const UFoliageResource *inResource) const;
	void PlaySoundEquip(const FItemStruct& inItem) const;
	void PlaySoundUnequip(const FItemStruct& inItem) const;
	void PlaySoundDropItem(const FItemStruct& inItem) const;
	void PlaySoundPickItem(const FItemStruct& inItem) const;
	void PlaySoundOpenDoor(const FItemStruct& inItem) const;
	void PlaySoundCloseDoor(const FItemStruct& inItem) const;
	void PlaySoundHarvest() const;

	// Inventory
	const FItemStruct* FindItem(const FString& Id) const override;
	AInteractableActor* DropItem(const FItemStruct& Item) override;
	void WearItem(FItemSlot& fromSlot);
	void ConsumeItem(FItemSlot& fromSlot);

	// Placing items
	void StartPlacingItem(FItemSlot& fromSlot);
	void CompletePlacingItem();
	void FinalizePlacingItem();
	void StartPlacingNextItemIfPossible(const FString& Id);
	void CancelPlaceItem();

	UInventory* GetInventory() const { return Inventory; }
	UCrafter* GetCrafter() const { return HandCrafter; }
	UInventory* GetEquippedItems() const { return EquippedItems; }

	FString GetDefaultSaveName() const;

protected:
	AInteractableActor* SpawnItem(const FItemStruct& Item, const FVector& inLocation, const FRotator& inRotation);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	/** Called for zooming input */
	void ZoomIn(const FInputActionValue& Value);
	void ZoomOut(const FInputActionValue& Value);
	void FreeLook(const FInputActionValue& Value);

	// Character actions
	void Interact(const FInputActionValue& Value);
	void Hit(const FInputActionValue& Value);
	void Consume(const FInputActionValue& Value);
	void Combine(const FInputActionValue& Value);
	void Pick(const FInputActionValue& Value);
	void Drop(const FInputActionValue& Value);

	void ToggleInventory(const FInputActionValue& Value);
	void ToggleCrafter(const FInputActionValue& Value);
	void ToggleMap(const FInputActionValue& Value);
	void ToggleInventoryUI();
	void ToggleCrafterUI();
	void Back(const FInputActionValue& Value);
	void Transfer(const FInputActionValue& Value);

	void ShiftModifierDown(const FInputActionValue& Value);
	void CtrlModifierDown(const FInputActionValue& Value);
	void ShiftModifierUp(const FInputActionValue& Value);
	void CtrlModifierUp(const FInputActionValue& Value);

	void QuickSaveGame(const FInputActionValue& Value);
	void QuickLoadGame(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Zoom Transforms", BlueprintReadWrite) TArray<float> ZoomTransforms;
	uint8 CurrentZoomLevel;

	void Interact_InInventory(const FInputActionValue& Value);
	void Interact_Voxel(const FInputActionValue& Value, const FVector &inTarget);
	void Interact_Interactable(const FInputActionValue& Value, AInteractableActor& inTarget);
	void Interact_Foliage(const FInputActionValue& Value, UInstancedStaticMeshComponent& inFoliageComponent, int32 inInstanceId);

	void GetItemDropPosition(FVector& outLocation, FRotator& outRotation, FVector &outThrowTowards) const;

	TArray<ContainedMaterial> CollectMaterialsFrom(const FVector& Location);
	void HitFoliageInstance(AInstancedFoliageActor& inFoliageActor, UFoliageResource& inFoliageComponent, int32 inInstanceId);

	void InteractDoor(AInteractableActor& inDoor);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ClearTimers();

	void SetupInventoryUI(APlayerController* pc);
	void SetupCrafterUI(APlayerController* pc);
	void SetupHUD(APlayerController* pc);
	void CheckBeginPlay();

	void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) AActor* CurrentTarget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) AInteractableActor* CurrentInteractable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FIntVector CurrentVoxel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) class AVoxelWorld *TargetVoxelWorld;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 CurrentTargetInstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInstancedStaticMeshComponent* CurrentTargetComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) class UBuildingSnapBox* CurrentBuildSnapBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) AInteractableActor* CurrentPlacedItem = nullptr;
	FItemSlot* CurrentPlacedItemFromSlot = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 CurrentPlacedItemElevation = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 CurrentPlacedItemRotation = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 PlacedItemElevationStep = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 PlacedItemRotationStep = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxPlaceItemDistance = 1000.0f;
	bool CheckCurrentPlacedItem(); // return true if placed item is active
	bool AllowPlaceItem(AInteractableActor& inItem, class UBuildingSnapBox *inSnapBox = nullptr) const;

	void CheckTarget();
	void SetCurrentTarget(AActor* target, UPrimitiveComponent* component = nullptr, int32 instanceId = -1);
	void SetHighlightIfInteractableTarget(AActor* target, bool value);

	// Input state
	bool ShowingMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
	bool ModifierShiftDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
	bool ModifierCtrlDown = false;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInventory	*Inventory = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInventory	*EquippedItems = nullptr;
	bool ShowingInventory = false;
	bool ShowingCrafter = false;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY() UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere) TSubclassOf<UCrafterWidget> CrafterWidgetClass;
	UPROPERTY() UCrafterWidget* CrafterWidget = nullptr;

	UPROPERTY(EditAnywhere) TSubclassOf<UHUDWidget> HUDWidgetClass;
	UPROPERTY() UHUDWidget* HUDWidget;

	UPROPERTY() UCrafter* HandCrafter = nullptr;
	bool ShowingHandCrafter = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> CraftableRecipies;

	UPROPERTY(EditAnywhere) TSubclassOf<UMapWidget> MapWidgetClass;
	UPROPERTY() UMapWidget* MapWidget;

	void CheckEnvironment();
	void EnsureNotUnderGround();
	void CheckCrafting(float DeltaSeconds);

	void CommitToHitAction();
	void HitExecute(AInstancedFoliageActor* inFoliageActor, UFoliageResource* inResourceComponent, int32 inInstanceId);
	FTimerHandle CommittedActionTimerHandle;
	bool CommittedToAction = false;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Console commands
	UFUNCTION(Exec, Category = ExecFunctions) void priadditem(const FString& Id);
	UFUNCTION(Exec, Category = ExecFunctions) void priadditems(const FString &Id, int Count);
	UFUNCTION(Exec, Category = ExecFunctions) void priresetmap();
	UFUNCTION(Exec, Category = ExecFunctions) void pridestroyallitems();
	UFUNCTION(Exec, Category = ExecFunctions) void prihelp();
	void CheatAddItems(const FString& Id, int Count);
};

extern FName GAME_TAG_DOOR;