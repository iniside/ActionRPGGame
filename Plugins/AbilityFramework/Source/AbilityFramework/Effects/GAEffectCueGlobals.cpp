#pragma once
#include "../AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "GAEffectCueGlobals.h"

void FGACueContainer::AddCue(const FGAEffectHandle& HandleIn, const FGAEffectCueParams& CueParamsIn)
{
	//if (!CueIn)
	//{
	//	return;
	//}
	//UGAEffectCue* Cue = NewObject<UGAEffectCue>(OwningComponent.Get(), CueIn);
	//Cues.Add(HandleIn, Cue);
	//Cue->SetParameters(OwningComponent->GetOwner(), CueParamsIn);
	//Cue->BeginCue();
}
void FGACueContainer::AddCue(const FGAEffectHandle& HandleIn)
{
	//Cues.Add(HandleIn, CueIn);
}
class UGAEffectCue* FGACueContainer::GetCue(const FGAEffectHandle& HandleIn)
{
	//UGAEffectCue* Cue = nullptr;
	//Cue = Cues.FindRef(HandleIn);
	return nullptr;
}
void FGACueContainer::ExecuteCue(const FGAEffectHandle& HandleIn)
{
	//UGAEffectCue* Cue = GetCue(HandleIn);
	//if (Cue)
	//{
	//	Cue->ExecuteCue();
	//}
}
void FGACueContainer::RemoveCue(const FGAEffectHandle& HandleIn)
{
	//UGAEffectCue* Cue;
	//if (Cues.Num() > 0)
	//{
	//	Cues.RemoveAndCopyValue(HandleIn, Cue);
	//	if (Cue)
	//	{
	//		Cue->EndCue();
	//		Cue->MarkPendingKill();
	//	}
	//}
}
void FGACueContainer::CueExpired(const FGAEffectHandle& HandleIn)
{

}