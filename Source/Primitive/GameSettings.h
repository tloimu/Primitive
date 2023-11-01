#pragma once

#include "CoreMinimal.h"
#include "GameSettings.generated.h"

USTRUCT(BlueprintType)
struct FGameSettings
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() FString itemsPath;
    UPROPERTY() FString classNamePrefix;
    UPROPERTY() FString iconNamePrefix;
};

USTRUCT(BlueprintType)
struct FSavedInventorySlot
{
    GENERATED_BODY()

    UPROPERTY() int slot;
    UPROPERTY() FString id;
    UPROPERTY() int count;
    UPROPERTY() float health;
};

USTRUCT(BlueprintType)
struct FSavedWearables
{
    GENERATED_BODY()

    UPROPERTY() FString id;
    UPROPERTY() FString on;
    UPROPERTY() uint32 wearOn;
    UPROPERTY() float health;
};

USTRUCT(BlueprintType)
struct FSavedItem
{
    GENERATED_BODY()

    UPROPERTY() FString id;
    UPROPERTY() uint32 itemRefId = 0;
    UPROPERTY() uint32 supportedBy = 0;
    UPROPERTY() FTransform transform;
    UPROPERTY() TArray<FSavedInventorySlot> slots; // for containers
    UPROPERTY() int state = 0;
};

USTRUCT(BlueprintType)
struct FSavedResource
{
    GENERATED_BODY()

    UPROPERTY() int32      id;
    UPROPERTY() FTransform transform;
};


USTRUCT(BlueprintType)
struct FSavedPlayer
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() FTransform transform;
    UPROPERTY() TArray<FSavedInventorySlot> slots;
    UPROPERTY() TArray<FSavedWearables> wear;

    UPROPERTY() float stamina;
    UPROPERTY() float health;
};

USTRUCT(BlueprintType)
struct FGameSave
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() TArray<FSavedItem> items;
    UPROPERTY() TArray<FSavedPlayer> players;
};
