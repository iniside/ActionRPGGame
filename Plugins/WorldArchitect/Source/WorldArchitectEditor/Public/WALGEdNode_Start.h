// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeGraphEditor/WALandscapeGraphEdNode.h"
#include "WALGEdNode_Start.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALGEdNode_Start : public UWALandscapeGraphEdNode
{
	GENERATED_BODY()
	
public:
	virtual	void AllocateDefaultPins() override;
	
	
};
