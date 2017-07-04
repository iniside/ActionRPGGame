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
	UPROPERTY(config, EditAnywhere, Category = "Cue Set")
		TAssetPtr<class UAFCueSet> DefaultCueSet;

	UPROPERTY()
		UAFCueSet* CueSet;
	
	//store per instigator ? Causer ? or global pool ?
	//stores unused instanced cues.
	TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>> InstancedCues;
	//store per instigator ? Causer ?
	//stores used cues.
	TMap<FObjectKey, TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>>> UsedCues;
public:
	void Initialize();
	void LoadCueSet();
#if WITH_EDITOR
	//handle clearing up cache when PIE mode is ending.
	void HandleOnPIEEnd(bool InVal);
#endif //WITH_EDITOR
public:
	static UAFCueManager* Get();
	void HandleCue(const FGameplayTagContainer& Tags,
		const FGAEffectCueParams& CueParams);
	void HandleRemoveCue(const FGameplayTagContainer& Tags,
		const FGAEffectCueParams& CueParams);
};
