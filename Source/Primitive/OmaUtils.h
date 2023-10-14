#include "CoreMinimal.h"

namespace OmaUtil
{
	bool TeleportActor(class AActor& inActor, FVector& inLocation, FRotator& inRotation);
	void DisablePhysicsAndCollision(class AActor& inActor);
	void EnablePhysicsAndCollision(class AActor& inActor);
}
