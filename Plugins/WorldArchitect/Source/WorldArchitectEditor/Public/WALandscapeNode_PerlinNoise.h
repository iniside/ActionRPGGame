// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/WALandscapeNode.h"
#include "WANoise.h"
#include "WALandscapeNode_PerlinNoise.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALandscapeNode_PerlinNoise : public UWALandscapeNode
{
	GENERATED_BODY()
public:
	TArray<uint16> CachedHeightmap;
public:
	virtual TArray<uint16> GenerateHeightmap() override;

	uint16 PerlinNoise2D(float x, float y, float amp, int32 octaves, int32 px, int32 py)
	{
		float noise = 0.f;
		for (int octave = 1; octave < octaves; octave *= 2)
			noise += FNoise::pnoise(x*px*octave, y*py*octave, px, py) / octave;

		return USHRT_MAX / 2.f + amp*noise;
	}
	float PerlinNoise2DFloat(float x, float y, float amp, int32 octaves, int32 px, int32 py)
	{
		float noise = 0.f;
		for (int octave = 1; octave < octaves; octave *= 2)
			noise += FNoise::pnoise(x*px*octave, y*py*octave, px, py) / octave;

		return noise;
	}
};
