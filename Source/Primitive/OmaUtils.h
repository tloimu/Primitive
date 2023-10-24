#include "CoreMinimal.h"
#include "ItemDatabase.h"
#include "Components/Image.h"

namespace OmaUtil
{
	bool TeleportActor(class AActor& inActor, FVector& inLocation, FRotator& inRotation);
	void DisablePhysicsAndCollision(class AActor& inActor);
	void EnablePhysicsAndCollision(class AActor& inActor);
	void DisableCollision(class AActor& inActor);
	void EnableCollision(class AActor& inActor);

	UItemDatabase* GetItemDb(UGameInstance *Instance);

	class MinMax
	{
	public:
		MinMax(float min0 = 0.0f, float max0 = 0.0f) { min = min0; max = max0; }

		void Value(float v);
		void Log(const char* Text);

		float min, max;
	};

	class BitmapMaker
	{
	public:
		void Start(uint32 x, uint32 y);
		void Set(uint32 x, uint32 y, const FColor& color);
		UTexture2D* Finish();
		void FinishToImage(UImage& inImage);

	protected:
		uint8* DataPtr = nullptr;
		class UTexture2D* Texture = nullptr;
		uint32 RowLength = 0;
	};



	class MsTimer
	{
	public:
		int64 started = 0;
		int64 checked = 0;

		MsTimer();

		void Start();
		int32 Check();
		int32 SinceCheck();
		int32 Total();

		void LogAndCheck(const char* Text);
		void LogSinceCheck(const char* Text);
		void LogTotal(const char* Text);
	};

}
