// Fill out your copyright notice in the Description page of Project Settings.


#include "MapWidget.h"
#include "WorldGenOne.h"
#include "ImageUtils.h"
#include "Components/Image.h"
#include "OmaUtil.h"

void
UMapWidget::Setup()
{
}

void
UMapWidget::GenerateMap()
{
	auto gen = FWorldGenOneInstance::sGeneratorInstance.Get();
	if (!gen)
	{
		UE_LOG(LogTemp, Error, TEXT("MapWidget: No World Generator"));
		return;
	}

	OmaUtil::MsTimer timer;

	UE_LOG(LogTemp, Warning, TEXT("MapWidget::WorldSize = %d"), gen->WorldSize);
	auto mapSizeRange = gen->WorldSize / 2;
	auto mapSize = gen->WorldSize;
	uint32 imageSize = 1024;
	float mapDPerPixel = ((float) mapSize) / imageSize;
	
	if (MapImage && TemperatureMapImage && MoistureMapImage && HeightMapImage)
	{
		OmaUtil::BitmapMaker tex, texH, texT, texM;
		tex.Start(imageSize, imageSize);
		texH.Start(imageSize, imageSize);
		texT.Start(imageSize, imageSize);
		texM.Start(imageSize, imageSize);

		float minH = 0.0f, maxH = 0.0f;

		for (uint32 y = 0; y < imageSize; y++)
		{
			for (uint32 x = 0; x < imageSize; x++)
			{
				float mapX = x * mapDPerPixel - mapSizeRange;
				float mapY = y * mapDPerPixel - mapSizeRange;
				auto h = gen->GetTerrainHeight(mapX, mapY, 0);
				if (h < minH)
					minH = h;
				if (h > maxH)
					maxH = h;
				auto hi = (uint8) (127 + (h / gen->TerrainHeight) * 128);
				auto m = (uint8) (gen->GetMoisture(mapX, mapY, h));
				auto t = (uint8)(127 + 2 * gen->GetTemperature(mapX, mapY, h));

				tex.Set(x, y, FColor(0, t, m, 255));
				texH.Set(x, y, FColor(hi, hi, hi, 255));
				texT.Set(x, y, FColor(t >= 127 ? t : 0, 0, t < 127 ? t : 0, 255));
				texM.Set(x, y, FColor(0, 0, m, 255));
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("MapWidget:: MinH = %f, MaxH = %f"), minH, maxH);

		tex.FinishToImage(*MapImage);
		texH.FinishToImage(*HeightMapImage);
		texT.FinishToImage(*TemperatureMapImage);
		texM.FinishToImage(*MoistureMapImage);

		timer.LogTotal("MapWidget generater maps");
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No MapImage, TemperatureMapImage, HeightMapImage or MoistureMapImage"));
	}
}
