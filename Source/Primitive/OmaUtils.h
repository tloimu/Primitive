#include "CoreMinimal.h"
#include "ItemDatabase.h"

namespace OmaUtil
{
	bool TeleportActor(class AActor& inActor, FVector& inLocation, FRotator& inRotation);
	void DisablePhysicsAndCollision(class AActor& inActor);
	void EnablePhysicsAndCollision(class AActor& inActor);

	UItemDatabase* GetItemDb(UGameInstance *Instance);
}
