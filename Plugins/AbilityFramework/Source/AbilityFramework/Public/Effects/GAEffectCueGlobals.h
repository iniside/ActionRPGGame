#pragma once
#include "GameplayTagContainer.h"
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAEffectCueGlobals.generated.h"

USTRUCT()
struct FGACueContainer
{
	GENERATED_USTRUCT_BODY()
public:
	TWeakObjectPtr<class UAFAbilityComponent> OwningComponent;

	void AddCue(const FGAEffectHandle& HandleIn, const FGAEffectCueParams& CueParamsIn);
	
	void AddCue(const FGAEffectHandle& HandleIn);

	/* Return cue. Might return null so always check it first! */
	class UGAEffectCue* GetCue(const FGAEffectHandle& HandleIn);
	
	void ExecuteCue(const FGAEffectHandle& HandleIn);

	void RemoveCue(const FGAEffectHandle& HandleIn);

	void CueExpired(const FGAEffectHandle& HandleIn);
};