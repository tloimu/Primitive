#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStruct.generated.h"


UENUM(BlueprintType)
enum class BodyPart : uint8
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


USTRUCT(BlueprintType)
struct FItemStruct// : public FTableRowBase
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Weight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Quality;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<BodyPart> CanWearIn;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int MaxStackSize;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int MaxHealth;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int Health;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int ContainedSlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<class AInteractableActor> ItemClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> Icon;

};
