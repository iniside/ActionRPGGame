// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GAGlobalTypes.h"
#include "Effects/AFCueActor.h"
#include "AFCueManager.generated.h"


USTRUCT()
struct FAFCueData
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FGameplayTag CueTag;
	UPROPERTY()
		FPrimaryAssetId AssetId;
	UPROPERTY(Transient)
		UClass* CueClass;

	FAFCueData()
		: CueClass(nullptr)
	{}
};

UENUM()
enum EAFCueEvent
{
	Activated,
	Executed,
	Expire,
	Removed
};

struct FAFCueActorKey
{
	FObjectKey InstigatorKey;
	FObjectKey TargetKey;
	FGameplayTag CueTag;
	struct Target {};
	struct Instigator {};

	FAFCueActorKey(Instigator, UObject* InInstigator)
		: InstigatorKey(FObjectKey(InInstigator))
		, TargetKey(FObjectKey())
		, CueTag(FGameplayTag())
	{}

	FAFCueActorKey(Target, UObject* InTarget)
		: InstigatorKey(FObjectKey())
		, TargetKey(FObjectKey(InTarget))
		, CueTag(FGameplayTag())
	{}

	FAFCueActorKey(const FGameplayTag& Tag)
		: InstigatorKey(FObjectKey())
		, TargetKey(FObjectKey())
		, CueTag(Tag)
	{}

	FAFCueActorKey(UObject* InInstigator, UObject* InTarget, const FGameplayTag& Tag)
		: InstigatorKey(FObjectKey(InInstigator))
		, TargetKey(FObjectKey(InTarget))
		, CueTag(Tag)
	{}

	inline bool operator==(const FAFCueActorKey& Other) const
	{
		return InstigatorKey == Other.InstigatorKey
			&& TargetKey == Other.TargetKey
			&& CueTag == Other.CueTag;
	}

};

inline uint32 GetTypeHash(const FAFCueActorKey& Key)
{
	return GetTypeHash(Key.InstigatorKey) ^
		GetTypeHash(Key.TargetKey) ^
		GetTypeHash(Key.CueTag);
}

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

	UPROPERTY()
		UObjectLibrary* StaticCues;

	TArray<FAFCueData> Cues;
	TMap<FGameplayTag, int32> CuesMap;

	//Cues = Instigator+Target (optional) since Actor Cue might not have target (ie. Fire storm effect).
	TMap<FAFCueActorKey, AAFCueActor> InstancedCues;
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
	void HandleCue(const FGameplayTagContainer& Tags
		, const FGAEffectCueParams& CueParams
		, EAFCueEvent CueEvent);
	
	void HandleRemoveCue(const FGameplayTagContainer& Tags,
		const FGAEffectCueParams& CueParams, FAFCueHandle InHandle);
protected:
	void OnFinishedLoad(int32 Idx
		, FPrimaryAssetId InPrimaryAssetId
		, FGAEffectCueParams CueParams
		, EAFCueEvent CueEvent);

	void HandleCueEvent(UClass* InCueClass, const FGAEffectCueParams& InCueParams, EAFCueEvent CueEvent);
};
