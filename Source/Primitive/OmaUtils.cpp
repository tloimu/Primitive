#include "OmaUtils.h"
#include "PrimitiveGameInstance.h"
#include <GameFramework/Actor.h>
#include <Components/BoxComponent.h>
#include "ImageUtils.h"
#include "Framework/Application/NavigationConfig.h"


void
OmaUtil::UseTabNavigationInWidgets(bool DoUse)
{
	// Disabling prevent Widgets (like Inventory) to consume Tab-key events
	FNavigationConfig& NavigationConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig.bTabNavigation = DoUse;
}

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
OmaUtil::RotateActorAroundPoint(class AActor& inActor, const FVector& inPivotPoint, FRotator& inRotation)
{
	FVector o, box;
	inActor.GetActorBounds(true, o, box, false);
	auto &original = inActor.GetTransform();
	FTransform pivotTransform = FTransform(original.GetRotation(), inPivotPoint);
	FTransform dPivotToOriginal = original * pivotTransform.Inverse();
	auto newTf = FTransform(inRotation.GetInverse(), FVector(0, 0, 0)) * dPivotToOriginal * FTransform(inRotation, FVector(0, 0, 0)) * pivotTransform;
	inActor.SetActorTransform(newTf);
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


// ----------------------------------------------------------------
// class <MinMax>
// ----------------------------------------------------------------

void
OmaUtil::MinMax::Value(float v)
{
	if (v < min)
		min = v;
	if (v > max)
		max = v;
}

void
OmaUtil::MinMax::Log(const char* Text)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: min [%f], max [%f]"), *FString(Text), min, max);
}


// ----------------------------------------------------------------
// class <MsTimer>
// ----------------------------------------------------------------


OmaUtil::MsTimer::MsTimer()
{
	Start();
}

void
OmaUtil::MsTimer::Start()
{
	started = FDateTime::UtcNow().GetTicks();
	checked = FDateTime::UtcNow().GetTicks();
}

int32
OmaUtil::MsTimer::Check()
{
	auto check = SinceCheck();
	checked = FDateTime::UtcNow().GetTicks();
	return check;
}

int32
OmaUtil::MsTimer::SinceCheck()
{
	auto now = FDateTime::UtcNow().GetTicks();
	return (now - checked) / 10000;
}

int32
OmaUtil::MsTimer::Total()
{
	auto now = FDateTime::UtcNow().GetTicks();
	return (now - started) / 10000;
}

void
OmaUtil::MsTimer::LogAndCheck(const char* Text)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Last check %ld ms ago"), *FString(Text), Check());
}

void
OmaUtil::MsTimer::LogSinceCheck(const char* Text)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Last check %ld ms ago"), *FString(Text), SinceCheck());
}

void
OmaUtil::MsTimer::LogTotal(const char* Text)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Total %ld ms"), *FString(Text), Total());
}


// ----------------------------------------------------------------
// class <BitmapMaker>
// ----------------------------------------------------------------


void
OmaUtil::BitmapMaker::Start(uint32 x, uint32 y)
{
	DataPtr = nullptr;
	RowLength = x;
	Texture = UTexture2D::CreateTransient(x, y);
	if (Texture)
	{
		auto gmm = &Texture->GetPlatformData()->Mips[0];
		DataPtr = (uint8*)(gmm->BulkData.Lock(LOCK_READ_WRITE));
	}
}

void
OmaUtil::BitmapMaker::Set(uint32 x, uint32 y, const FColor& color)
{
	if (DataPtr)
	{
		auto pa = &DataPtr[y * RowLength * 4 + 4 * x];
		pa[0] = color.B;
		pa[1] = color.G;
		pa[2] = color.R;
		pa[3] = color.A;
	}
}

UTexture2D*
OmaUtil::BitmapMaker::Finish()
{
	DataPtr = nullptr;
	if (Texture)
	{
		auto gmm = &Texture->GetPlatformData()->Mips[0];
		gmm->BulkData.Unlock();
	}

	auto t = Texture;
	Texture = nullptr;
	return t;
}

void
OmaUtil::BitmapMaker::FinishToImage(UImage& inImage)
{
	auto t = Finish();
	t->UpdateResource();
	inImage.SetBrushFromSoftTexture(t);
}
