#pragma once
#include "GameplayTagContainer.h"
#include "GAEffectCue.h"
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAEffectCueGlobals.generated.h"

USTRUCT()
struct FGACueContainer
{
	GENERATED_USTRUCT_BODY()
public:
	TWeakObjectPtr<class UGAAttributeComponent> OwningComponent;
	
	TMap<FGAGameEffectHandle, UGAEffectCue*> Cues;

	void AddCue(const FGAGameEffectHandle& HandleIn, TSubclassOf<UGAEffectCue> CueIn, const FGAEffectCueParams& CueParamsIn);
	
	void AddCue(const FGAGameEffectHandle& HandleIn, UGAEffectCue* CueIn);

	class UGAEffectCue* GetCue(const FGAGameEffectHandle& HandleIn);
	
	void ExecuteCue(const FGAGameEffectHandle& HandleIn);

	/* Return cue. Might return null so always check it first! */
	void RemoveCue(const FGAGameEffectHandle& HandleIn);

	void CueExpired(const FGAGameEffectHandle& HandleIn);
};