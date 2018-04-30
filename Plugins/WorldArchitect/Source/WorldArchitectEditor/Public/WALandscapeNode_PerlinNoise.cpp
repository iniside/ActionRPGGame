// Fill out your copyright notice in the Description page of Project Settings.

#include "WALandscapeNode_PerlinNoise.h"



TArray<uint16> UWALandscapeNode_PerlinNoise::GenerateHeightmap()
{
	TArray<uint16> RetVal;
	if (bDirty)
	{
		return RetVal;
	}
	else
	{
		return CachedHeightmap;
	}
}
