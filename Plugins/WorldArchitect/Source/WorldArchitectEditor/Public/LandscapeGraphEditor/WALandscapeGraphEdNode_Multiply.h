// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeGraphEditor/WALandscapeGraphEdNode.h"
#include "WALandscapeGraphEdNode_Multiply.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALandscapeGraphEdNode_Multiply : public UWALandscapeGraphEdNode
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UWALandscapeGraphEdNode* InputA;
	UPROPERTY()
		UWALandscapeGraphEdNode* InputB;
public:
	virtual	void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void GenerateHeightMap(TArray<uint16>& InOutMap) override;
};
