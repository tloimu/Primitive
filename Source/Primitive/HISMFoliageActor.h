#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HISMFoliageActor.generated.h"

UCLASS()
class PRIMITIVE_API AHISMFoliageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHISMFoliageActor();

protected:
	virtual void BeginPlay() override;
};
