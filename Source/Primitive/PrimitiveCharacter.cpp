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
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <Primitive/Interactable.h>


//////////////////////////////////////////////////////////////////////////
// APrimitiveCharacter

APrimitiveCharacter::APrimitiveCharacter()
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
	ShowingInventory = false;

	InventoryWidgetClass = nullptr;
	InventoryWidget = nullptr;

	HUDWidgetClass = nullptr;
	HUDWidget = nullptr;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void APrimitiveCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

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
			check(InventoryWidget);
			InventoryWidget->Player = this;

			HUDWidget = CreateWidget<UHUDWidget>(pc, HUDWidgetClass);
			check(HUDWidget);
			HUDWidget->AddToPlayerScreen();
		}
	}
}

void APrimitiveCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
			SetCurrentTarget(hit);
		else
			SetCurrentTarget(nullptr);
	}
	else
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), end, 5, 5, FLinearColor::Red);
		SetCurrentTarget(nullptr);
	}
}

void APrimitiveCharacter::SetCurrentTarget(AActor* target)
{
	if (CurrentTarget != target)
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
			UE_LOG(LogTemp, Warning, TEXT("Target %s"), *name);
		}
		CurrentTarget = target;
		
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
			item.Icon.LoadSynchronous(); // Force resolve lazy loading - not sure if this is the best way but seems to make it work. Maybe check icon.IsPending() to see the need to load
			if (InventoryWidget->AddItem(item))
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
	// ???? TODO:
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
}

void APrimitiveCharacter::Interact(const FInputActionValue& Value)
{
	if (CurrentInteractable)
	{
		IInteractable::Execute_Interact(CurrentInteractable);
	}
}

void APrimitiveCharacter::ToggleInventory(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory %d"), Value.Get<bool>());
	if (ShowingInventory)
		return;

	auto pc = GetController<APlayerController>();
	ShowingInventory = true;
	if (InventoryWidget != nullptr)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory ON"));
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

void APrimitiveCharacter::CreateDroppedItem(const FItemStruct& Item)
{
	FVector start = GetActorLocation();
	FVector end = start + FollowCamera->GetForwardVector().GetSafeNormal() * 50.0f;
	auto rotation = GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto itemActor = GetWorld()->SpawnActor<AInteractableActor>(Item.ItemClass, end, rotation, SpawnInfo);
}
