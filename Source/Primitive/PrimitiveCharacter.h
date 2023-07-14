// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InventoryWidget.h"
#include "HUDWidget.h"
#include "GameSettings.h"
#include "PrimitiveCharacter.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "Zoom Transforms", BlueprintReadWrite) TArray<float> ZoomTransforms;
	uint8 CurrentZoomLevel;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ReadConfigFiles();
	void UpdateItemSettingsClass(FItemSettings& item);
	void ReadGameSave();
	void SpawnItem(const FSavedItem& item);

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TMap<FString, FItemSettings> ItemSettings;

	void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) AActor* CurrentTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) AInteractableActor* CurrentInteractable;

	void CheckTarget();
	void SetCurrentTarget(AActor* target);
	void SetHighlightIfInteractableTarget(AActor* target, bool value);

	bool ShowingInventory;

	UPROPERTY(EditAnywhere) TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY() UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere) TSubclassOf<UHUDWidget> HUDWidgetClass;
	UPROPERTY() UHUDWidget* HUDWidget;

/*	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;*/

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
