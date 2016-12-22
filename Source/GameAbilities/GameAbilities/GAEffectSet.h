// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GAEffectSet.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGAEffectSet : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class UGAGameEffectSpec>> Specs;
	
	
};
