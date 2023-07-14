#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStruct.generated.h"


UENUM(BlueprintType)
enum class BodyPart : uint8
{
    Head = 0            UMETA(DisplayName = "Head"),
    PrimaryHand = 1     UMETA(DisplayName = "PrimaryHand"),
    SecondaryHand = 2   UMETA(DisplayName = "SecondaryHand"),
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
struct FItemStruct : public FTableRowBase
{
    GENERATED_BODY()

    FItemStruct() : Weight(0.0f), Quality(1.0f), MaxStackSize(1) {}

    bool operator== (const FItemStruct rhs) const
    {
        if (Id == rhs.Id)
        {
            return true;
        }
        else return false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Id")) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Name")) FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Weight")) float Weight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Quality;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<BodyPart> CanWearIn;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<class AInteractableActor> ItemClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture> Icon;

};
