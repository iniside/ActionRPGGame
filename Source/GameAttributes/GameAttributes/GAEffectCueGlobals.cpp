#pragma once
#include "GameAttributes.h"
#include "GAEffectCue.h"
#include "GAAttributeComponent.h"
#include "GAEffectCueGlobals.h"

void FGACueContainer::AddCue(const FGAGameEffectHandle& HandleIn, TSubclassOf<UGAEffectCue> CueIn, const FGAEffectCueParams& CueParamsIn)
{
	if (!CueIn)
	{
		return;
	}
	UGAEffectCue* Cue = NewObject<UGAEffectCue>(OwningComponent.Get(), CueIn);
	Cues.Add(HandleIn, Cue);
	Cue->SetParameters(OwningComponent->GetOwner(), CueParamsIn);
	Cue->BeginCue();
}
void FGACueContainer::AddCue(const FGAGameEffectHandle& HandleIn, UGAEffectCue* CueIn)
{
	Cues.Add(HandleIn, CueIn);
}
class UGAEffectCue* FGACueContainer::GetCue(const FGAGameEffectHandle& HandleIn)
{
	UGAEffectCue* Cue = nullptr;
	Cue = Cues.FindRef(HandleIn);
	return Cue;
}
void FGACueContainer::ExecuteCue(const FGAGameEffectHandle& HandleIn)
{
	UGAEffectCue* Cue = GetCue(HandleIn);
	if (Cue)
	{
		Cue->ExecuteCue();
	}
}
void FGACueContainer::RemoveCue(const FGAGameEffectHandle& HandleIn)
{
	UGAEffectCue* Cue;
	Cues.RemoveAndCopyValue(HandleIn, Cue);
	if (Cue)
	{
		Cue->EndCue();
	}
}
void FGACueContainer::CueExpired(const FGAGameEffectHandle& HandleIn)
{

}