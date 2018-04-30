// Fill out your copyright notice in the Description page of Project Settings.

#include "WALandscapeNode_Multiply.h"



TArray<uint16> UWALandscapeNode_Multiply::GenerateHeightmap()
{
	TArray<uint16> OutputA;
	TArray<uint16> OutputB;
	TArray<uint16> RetVal;

	if (InputA && InputB)
	{
		OutputA = InputA->GenerateHeightmap();
	
		OutputB = InputB->GenerateHeightmap();
	
		
		RetVal.Init(0, OutputA.Num());
		for (int32 Idx = 0; Idx < OutputA.Num(); Idx++)
		{
			RetVal[Idx] = OutputA[Idx] * OutputB[Idx];
		}

	}
	return RetVal;
}
