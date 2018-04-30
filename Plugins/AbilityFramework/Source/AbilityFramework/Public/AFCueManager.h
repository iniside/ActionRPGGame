// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GAGlobalTypes.h"
#include "Effects/GAEffectCue.h"
#include "AFCueManager.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class ABILITYFRAMEWORK_API UAFCueManager : public UObject
{
	GENERATED_BODY()
protected:
	//UPROPERTY()
		static UAFCueManager* ManagerInstance;
		UWorld* CurrentWorld;
	
	//store per instigator ? Causer ? or global pool ?
	//stores unused instanced cues.
	TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>> InstancedCues;
	//store per instigator ? Causer ?
	//stores used cues.
	TMap<FObjectKey, TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>>> UsedCues;

	TMap<FAFCueHandle, AGAEffectCue*> ActiveCues;

public:
	void Initialize();
#if WITH_EDITOR
	//handle clearing up cache when PIE mode is ending.
	void HandleOnPIEEnd(bool InVal);
#endif //WITH_EDITOR
	void HandlePreLoadMap(const FString& InMapName);
	void HandlePostLoadMap(UWorld* InWorld);
public:
	static UAFCueManager* Get();
	void HandleCue(const FGameplayTagContainer& Tags,
		const FGAEffectCueParams& CueParams, FAFCueHandle InHandle);
	void HandleExecuteCue(FAFCueHandle InHandle);
	void HandleRemoveCue(const FGameplayTagContainer& Tags,
		const FGAEffectCueParams& CueParams, FAFCueHandle InHandle);
protected:
	void OnFinishedLoad(FGameplayTag InCueTag
		, FPrimaryAssetId InPrimaryAssetId
		, FGAEffectCueParams CueParams
		, FAFCueHandle InHandle);
};
