// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "AFCueSet.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFCueSet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, TSubclassOf<class AGAEffectCue>> Cues;
	
	
};
