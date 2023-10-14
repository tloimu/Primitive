#include "OmaUtils.h"
#include <GameFramework/Actor.h>

bool
OmaUtil::TeleportActor(AActor& inActor, FVector& inLocation, FRotator& inRotation)
{
	auto ok = inActor.SetActorLocationAndRotation(inLocation, inRotation, false, nullptr, ETeleportType::TeleportPhysics);
	if (ok)
	{
		for (UActorComponent* Component : inActor.GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				PrimComp->SetWorldLocationAndRotation(inLocation, inRotation);
			}
		}
	}
	return ok;
}
