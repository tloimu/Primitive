// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemStruct.h"
#include "Craftable.h"
#include "ItemDatabase.generated.h"


USTRUCT(BlueprintType)
struct FItemSpec
{
    GENERATED_BODY()

    FItemSpec() {}

    bool operator== (const FItemSpec rhs) const
    {
        if (Id == rhs.Id)
        {
            return true;
        }
        else return false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<class AInteractableActor> ItemClass;
};


UCLASS()
class PRIMITIVE_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UItemDatabase(const FObjectInitializer& Init);

	void SetupItems();

	const FItemStruct* FindItem(const FString& Id) const;
    const FCraftRecipie* FindRecipie(const FString& Id) const;

protected:
    UPROPERTY(EditAnywhere) TArray<FItemSpec> ItemSpecs;
    UPROPERTY(EditAnywhere) TArray<FCraftRecipie> RecipieSpecs;

    TMap<FString, FItemStruct> Items;
    TMap<FString, FCraftRecipie> Recipies;
};
