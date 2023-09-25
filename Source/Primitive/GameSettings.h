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
struct FSavedItem
{
    GENERATED_BODY()

    UPROPERTY() FString id;
    UPROPERTY() TArray<float> location;
    UPROPERTY() TArray<float> rotation;
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
    UPROPERTY() float health;
};

USTRUCT(BlueprintType)
struct FSavedPlayer
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() TArray<FSavedInventorySlot> slots;
    UPROPERTY() TArray<FSavedWearables> wear;
};

USTRUCT(BlueprintType)
struct FGameSave
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() TArray<FSavedItem> items;
    UPROPERTY() TArray<FSavedPlayer> players;
};
