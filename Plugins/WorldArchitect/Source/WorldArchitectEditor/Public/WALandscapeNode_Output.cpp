// Fill out your copyright notice in the Description page of Project Settings.

#include "WALandscapeNode_Output.h"




TArray<uint16> UWALandscapeNode_Output::GenerateHeightmap()
{
	TArray<uint16> RetVal;
	if (Heightmap)
	{
		RetVal = Heightmap->GenerateHeightmap();
	}
	return RetVal;
}