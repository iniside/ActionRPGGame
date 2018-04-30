// Fill out your copyright notice in the Description page of Project Settings.

#include "WALGEdNode_PerlinNoise.h"
#include "LandscapeEditorUtils.h"
#include "LandscapeInfoMap.h"
#include "LandscapeStreamingProxy.h"

void UWALGEdNode_PerlinNoise::GenerateHeightMap(TArray<uint16>& InOutMap)
{
	//GEditor->GetWorld();
	//TArray<ALandscape*> Landscapes;
	//for (TActorIterator<ALandscape> LIter(GetEditorMode()->GetWorld()); LIter; ++LIter)
	//{
	//	Landscapes.Add(*LIter);
	//}
	//auto& LandscapeInfoMap = ULandscapeInfoMap::GetLandscapeInfoMap(GetEditorMode()->GetWorld());


	//if (Landscapes.Num() > 0)
	//{
	//	FIntRect bounds = Landscapes[0]->GetBoundingRect();

	//	int32 numHeights = (bounds.Width() + 1)*(bounds.Height() + 1);
	//	TArray<uint16> Data;
	//	Data.Init(0, numHeights);
	//	TArray<float> DataFloat;
	//	DataFloat.Init(0, numHeights);
	//	TArray<float> DataFloat2;
	//	DataFloat2.Init(0, numHeights);
	//	TArray<float> DataFloat3;
	//	DataFloat3.Init(0, numHeights);

	//	int32 cols = bounds.Width() + 1, rows = bounds.Height() + 1;
	//	int32 octaves = 16, px = 4, py = 4;
	//	float amplitude = 20000.f;
	//	//for (int i = 0; i < Data.Num(); i++) 
	//	//{
	//	//	float nx = (i % cols) / (float)cols; //normalized col
	//	//	float ny = (i / cols) / (float)rows; //normalized row
	//	//	Data[i] = FMath::Clamp<uint16>(PerlinNoise2D(nx, ny, amplitude, octaves, px, py), 0, 32768);
	//	//}
	//	for (int i = 0; i < DataFloat.Num(); i++)
	//	{
	//		float nx = (i % cols) / (float)cols; //normalized col
	//		float ny = (i / cols) / (float)rows; //normalized row
	//		DataFloat[i] = PerlinNoise2DFloat(nx, ny, amplitude, octaves, px, py);
	//		DataFloat2[i] = PerlinNoise2DFloat(nx, ny, amplitude, 4, 12, 12);
	//		DataFloat3[i] = DataFloat[i] * DataFloat2[i];
	//	}
	//	for (int i = 0; i < Data.Num(); i++)
	//	{
	//		Data[i] = (USHRT_MAX / 2.f) + (DataFloat3[i] * amplitude);
	//	}
	//	InOutMap = Data;
	//}
}
void UWALGEdNode_PerlinNoise::AllocateDefaultPins()
{

}
void UWALGEdNode_PerlinNoise::PinConnectionListChanged(UEdGraphPin* Pin)
{

}