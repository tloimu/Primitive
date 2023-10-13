#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStruct.generated.h"


UENUM(BlueprintType)
enum class BodyPart : uint8 // ???? TODO: Renamve to <EBodyPart>
{
    None = 0            UMETA(DisplayName = "None"),
    Head = 1            UMETA(DisplayName = "Head"),
    Hand = 2            UMETA(DisplayName = "Hand"),
    TwoHanded = 3       UMETA(DisplayName = "TwoHanded"),
    Feet = 4            UMETA(DisplayName = "Feet"),
    Torso = 5           UMETA(DisplayName = "Torso"),
    Legs = 6            UMETA(DisplayName = "Legs"),
    Neck = 7            UMETA(DisplayName = "Neck"),
    Finger = 8          UMETA(DisplayName = "Finger"),
    Back = 9            UMETA(DisplayName = "Back"),
    Hips = 10           UMETA(DisplayName = "Hips"),
    Wrist = 11          UMETA(DisplayName = "Wrist"),
    Arm = 12            UMETA(DisplayName = "Arm")
};


UENUM(BlueprintType)
enum class ToolUtility : uint8 // ???? TODO: Rename to <EItemUtility>
{
    None = 0                    UMETA(DisplayName = "None"),
    Cut = 1                     UMETA(DisplayName = "Cut"),
    Dig = 2                     UMETA(DisplayName = "Dig"),
    FarmSoftResource = 11       UMETA(DisplayName = "FarmSoftResource"),
    FarmHardResource = 12       UMETA(DisplayName = "FarmHardResource"),
    FarmVeryHardResource = 13   UMETA(DisplayName = "FarmVeryHardResource"),
    ContainLiquid = 20          UMETA(DisplayName = "ContainLiquid"),
    ContainPowder = 21          UMETA(DisplayName = "ContainPowder"),
    Building = 30               UMETA(DisplayName = "Building"),
};

UENUM(BlueprintType)
enum class ItemForm : uint8 // ???? TODO: Rename to <EItemForm>
{
    None = 0       UMETA(DisplayName = "None"),
    Long = 1       UMETA(DisplayName = "Long"), // Long itemst that do not fit into backpack, chest etc. e.g. logs or long beams
    Powder = 2     UMETA(DisplayName = "Powder"), // Poweder form item e.g. sand or flour
    Liquid = 3     UMETA(DisplayName = "Liquid"), // Liquid form item e.g. water, milk, honey, oil
    Gas = 4        UMETA(DisplayName = "Gas") // Gas form item e.g. air, methane
};


USTRUCT(BlueprintType)
struct FItemStruct
{
    GENERATED_BODY()

    FItemStruct() : Weight(0.0f), Quality(1.0f), MaxStackSize(1), MaxHealth(1), Health(1), ContainedSlots(0) {}

    bool operator== (const FItemStruct rhs) const
    {
        if (Id == rhs.Id && Quality == rhs.Quality)
        {
            return true;
        }
        else return false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Weight = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Quality = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int MaxStackSize = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int MaxHealth = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<ItemForm> RequiresStorageFor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<class AInteractableActor> ItemClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> Icon;

    // Instances
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int Health = 1;

    // Tools and equipment
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<BodyPart> CanWearIn;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<ToolUtility> UsableFor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> CraftableRecipies;

    // Containers
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int ContainedSlots = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<ItemForm> SlotCapability;

    // Consumables
    bool IsConsumable() const { return (ConsumedWater > 0.0f || ConsumedFood > 0.0f || ConsumedAir > 0.0f); }

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ConsumedWater = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ConsumedFood = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ConsumedAir = 0.0f;
};
