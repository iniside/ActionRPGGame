// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/WALandscapeNode.h"
#include "WALandscapeNode_Multiply.generated.h"

/**
 * 
 */
UCLASS()
class WORLDARCHITECTEDITOR_API UWALandscapeNode_Multiply : public UWALandscapeNode
{
	GENERATED_BODY()
protected:
	TArray<uint16> InputACache;
	TArray<uint16> InputBCache;
public:
	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
		UWALandscapeNode* InputA;
	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
		UWALandscapeNode* InputB;
	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
		UWALandscapeNode* Output;

	virtual TArray<uint16> GenerateHeightmap() override;
	
};
