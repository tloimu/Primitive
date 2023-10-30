#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSettings.h"
#include <Voxel/Public/VoxelTools/VoxelDataTools.h>
#include "PrimitiveSaveGame.generated.h"

UCLASS()
class PRIMITIVE_API UPrimitiveSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY() uint32					Version = 0;
	UPROPERTY() TArray<FSavedPlayer>	Players;
	UPROPERTY() TArray<FSavedItem>		Items;
	UPROPERTY() FVoxelCompressedWorldSave Voxels;
};
