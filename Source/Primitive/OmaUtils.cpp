#include "OmaUtils.h"
#include "PrimitiveGameInstance.h"
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
				PrimComp->SetWorldLocationAndRotation(inLocation, inRotation);
			}
		}
	}
	return ok;
}

void
OmaUtil::DisablePhysicsAndCollision(AActor& inActor)
{
	for (UActorComponent* Component : inActor.GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
			PrimComp->SetSimulatePhysics(false);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	inActor.SetActorEnableCollision(false);
}

void
OmaUtil::EnablePhysicsAndCollision(AActor& inActor)
{
	for (UActorComponent* Component : inActor.GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
			PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PrimComp->SetSimulatePhysics(true);
		}
	}
	inActor.SetActorEnableCollision(true);
}


UItemDatabase*
OmaUtil::GetItemDb(UGameInstance* Instance)
{
	auto gi = Cast<UPrimitiveGameInstance>(Instance);
	if (gi)
		return gi->ItemDb;
	else
		return nullptr;
}
