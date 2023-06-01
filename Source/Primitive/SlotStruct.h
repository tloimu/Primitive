#pragma once

#include "CoreMinimal.h"
#include "SlotStruct.generated.h"


USTRUCT(BlueprintType)
struct FSlotStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int Count;
};
