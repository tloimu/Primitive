#pragma once

#include "CoreMinimal.h"
#include "GameSettings.generated.h"


USTRUCT(BlueprintType)
struct FItemSettings
{
    GENERATED_BODY()

    UPROPERTY() FString id;
    UPROPERTY() FString className;
    UPROPERTY() FString icon;
    UPROPERTY() TSubclassOf<class AInteractableActor> ItemClass;
};

USTRUCT(BlueprintType)
struct FGameSettings
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() FString itemsPath;
    UPROPERTY() FString classNamePrefix;
    UPROPERTY() FString iconNamePrefix;

    UPROPERTY() TArray<FItemSettings> items;
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
struct FGameSave
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() TArray<FSavedItem> items;
};
