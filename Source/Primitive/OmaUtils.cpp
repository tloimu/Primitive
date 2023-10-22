#include "OmaUtils.h"
#include "PrimitiveGameInstance.h"
#include <GameFramework/Actor.h>
#include <Components/BoxComponent.h>

bool
OmaUtil::TeleportActor(AActor& inActor, FVector& inLocation, FRotator& inRotation)
{
	auto ot = inActor.GetActorTransform();
	auto ol = inLocation - inActor.GetActorLocation();
	auto or = inRotation - inActor.GetActorRotation();
	auto loc = inActor.GetActorLocation();
	auto dt = FTransform(or , ol);

	if (ol.IsZero() && or.IsZero())
		return true;

	inActor.AddActorWorldTransformKeepScale(dt);
	auto ok = true;
	if (ok)
	{
		UE_LOG(LogTemp, Warning, TEXT("Move Actor [%s] to [%f, %f, %f] by [%f, %f, %f] scale [%f, %f, %f]"), *inActor.GetName(),
			loc.X, loc.Y, loc.Z,
			dt.GetLocation().X, dt.GetLocation().Y, dt.GetLocation().Z,
			dt.GetScale3D().X, dt.GetScale3D().Y, dt.GetScale3D().Z);
		for (UActorComponent* Component : inActor.GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				auto snap = Cast<UBoxComponent>(PrimComp);
				auto mesh = Cast<UStaticMeshComponent>(PrimComp);
				loc = PrimComp->GetComponentLocation(); 
				if (mesh)
				{
					//PrimComp->AddWorldTransformKeepScale(dt);
					UE_LOG(LogTemp, Warning, TEXT(" - move mesh component %s from [%f, %f, %f]"), *PrimComp->GetName(), loc.X, loc.Y, loc.Z);
				}
				// PrimComp->AddWorldTransformKeepScale(dt);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" - not move component %s"), *Component->GetName());
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
			auto snap = Cast<UBoxComponent>(PrimComp);
			if (snap)
			{
				UE_LOG(LogTemp, Warning, TEXT(" - box component %s"), *PrimComp->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
				PrimComp->SetSimulatePhysics(false);
				//PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
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
			auto snap = Cast<UBoxComponent>(PrimComp);
			if (snap)
			{
				UE_LOG(LogTemp, Warning, TEXT(" - box component %s"), *PrimComp->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
				//PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				PrimComp->SetSimulatePhysics(true);
			}
		}
	}
	inActor.SetActorEnableCollision(true);
}


void
OmaUtil::DisableCollision(AActor& inActor)
{
	for (UActorComponent* Component : inActor.GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			auto snap = Cast<UBoxComponent>(PrimComp);
			if (snap)
			{
				UE_LOG(LogTemp, Warning, TEXT(" - box component %s"), *PrimComp->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
				PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	inActor.SetActorEnableCollision(false);
}

void
OmaUtil::EnableCollision(AActor& inActor)
{
	for (UActorComponent* Component : inActor.GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			auto snap = Cast<UBoxComponent>(PrimComp);
			if (snap)
			{
				UE_LOG(LogTemp, Warning, TEXT(" - box component %s"), *PrimComp->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" - component %s"), *PrimComp->GetName());
				PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
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
