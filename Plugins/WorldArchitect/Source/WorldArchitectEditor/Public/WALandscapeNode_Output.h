// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/WALandscapeNode.h"
#include "WALandscapeNode_Output.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALandscapeNode_Output : public UWALandscapeNode
{
	GENERATED_BODY()
public:
	TArray<uint16> CachedHeightmap;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
		UWALandscapeNode* Heightmap;

public:
	virtual TArray<uint16> GenerateHeightmap() override;
};
