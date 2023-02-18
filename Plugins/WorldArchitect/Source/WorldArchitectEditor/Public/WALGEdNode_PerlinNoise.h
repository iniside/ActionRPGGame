// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WANoise.h"
#include "LandscapeGraphEditor/WALandscapeGraphEdNode.h"
#include "WALGEdNode_PerlinNoise.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALGEdNode_PerlinNoise : public UWALandscapeGraphEdNode
{
	GENERATED_BODY()
	
	virtual void GenerateHeightMap(TArray<uint16>& InOutMap) override;
	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

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
