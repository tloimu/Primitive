// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Engine/SkyLight.h"
#include "InputActionValue.h"
#include "InventoryWidget.h"
#include "HUDWidget.h"
#include "GameSettings.h"
#include "Inventory.h"
#include "FoliageResource.h"
#include "PrimitiveCharacter.generated.h"

struct ContainedMaterial
{
	int32 index;
	float amount;
};

class FWorldGenOneInstance;

UCLASS(config=Game)
class APrimitiveCharacter : public ACharacter
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


public:
	APrimitiveCharacter();
	
	void CreateDroppedItem(const FItemStruct& Item);

protected:

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
	void Back(const FInputActionValue& Value);
	void Transfer(const FInputActionValue& Value);

	void ShiftModifierDown(const FInputActionValue& Value) { ModifierShiftDown = true; }
	void CtrlModifierDown(const FInputActionValue& Value) { ModifierCtrlDown = true; }
	void ShiftModifierUp(const FInputActionValue& Value) { ModifierShiftDown = false; }
	void CtrlModifierUp(const FInputActionValue& Value) { ModifierCtrlDown = false; }

	UPROPERTY(EditAnywhere, Category = "Zoom Transforms", BlueprintReadWrite) TArray<float> ZoomTransforms;
	uint8 CurrentZoomLevel;

	TArray<ContainedMaterial> CollectMaterialsFrom(const FVector& Location);
	void HitFoliageInstance(AInstancedFoliageActor& inFoliageActor, UFoliageResource& inFoliageComponent, int32 inInstanceId);
	void EquipItem(UInventorySlot& FromSlot, UInventorySlot &ToSlot);
	void UnequipItem(UInventorySlot& FromSlot, UInventorySlot& ToSlot);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupInventoryUI(APlayerController* pc);
	void SetupHUD(APlayerController* pc);
	void CheckBeginPlay();

	void ReadConfigFiles();
	const FItemStruct* FindItem(const FString& Id) const;
	//void UpdateItemSettingsClass(FItemSettings& item);
	void ReadGameSave();
	AInteractableActor* SpawnItem(const FSavedItem& item);
	void SetSavedInventorySlot(const FSavedInventorySlot& saved, FItemSlot& slot);
	void SetSavedContainerSlots(UInventory* inInventory, const FSavedItem& saved);

	//UPROPERTY(EditAnywhere, BlueprintReadOnly) TMap<FString, FItemSettings> ItemSettings;

	void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) AActor* CurrentTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) AInteractableActor* CurrentInteractable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FIntVector CurrentVoxel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) class AVoxelWorld *TargetVoxelWorld;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 CurrentTargetInstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInstancedStaticMeshComponent* CurrentTargetComponent;

	void CheckTarget();
	void SetCurrentTarget(AActor* target, UPrimitiveComponent* component = nullptr, int32 instanceId = -1);
	void SetHighlightIfInteractableTarget(AActor* target, bool value);

	// Input state

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
	bool ModifierShiftDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
	bool ModifierCtrlDown = false;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInventory	*Inventory = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UInventory	*EquippedItems = nullptr;
	bool ShowingInventory;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY() UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere) TSubclassOf<UHUDWidget> HUDWidgetClass;
	UPROPERTY() UHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UItemDatabase* ItemDb;

	// Setting up the Map

	UPROPERTY(EditAnywhere) TSubclassOf<class UHUDWidget> WorldGeneratorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool DoGenerateFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 MaxFoliageInstances = 200000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 MaxFoliageRange = 20000;

	void GenerateFoilage();

	void CheckEnvironment();
	void EnsureNotUnderGround();
	void CheckSunlight(float DeltaSeconds);

/*	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;*/

	// Environmental variables and functionality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float ClockInSecs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	int32 Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	int32 DayOfYear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float ClockSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<ASkyLight> SkyLight;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
