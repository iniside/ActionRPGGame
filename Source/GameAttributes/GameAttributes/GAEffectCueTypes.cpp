#pragma once
#include "GameAttributes.h"
#include "GAEffectCue.h"
#include "GAAttributeComponent.h"
#include "GAEffectCueTypes.h"

void FGAActiveEffectCue::OnActivated()
{
	//enable preditive ticking, for cues ?
	FTimerDelegate periodDel = FTimerDelegate::CreateRaw(this, &FGAActiveEffectCue::OnPeriod);
	Context.Target->GetWorldTimerManager().SetTimer(PeriodTimerHandle, periodDel, Period, true);

	OnApplied();
}

void FGAActiveEffectCue::OnApplied()
{
	if (EffectCue.IsValid())
		EffectCue->OnEffectApplied();
}

void FGAActiveEffectCue::OnPeriod()
{
	if (EffectCue.IsValid())
		EffectCue->OnEffectPeriod();
}

void FGAActiveEffectCue::OnRemoved()
{
	if (EffectCue.IsValid())
		EffectCue->OnEffectRemoved();

	//Context.TargetComp->RemoveEffectCue(MyHandle.GetHandle());
}

void FGAActiveEffectCue::OnEnded()
{
	if (EffectCue.IsValid())
		EffectCue->OnEffectEnded();

	//Context.TargetComp->RemoveEffectCue(MyHandle.GetHandle());
}

void FGAActiveEffectCue::CleanUp()
{
	
}

void FGAActiveEffectCue::CueRemoved()
{
	OnRemoved();
	Context.Target->GetWorldTimerManager().ClearTimer(PeriodTimerHandle);
}

void FGAActiveEffectCue::CueExpired()
{
	OnEnded();
	Context.Target->GetWorldTimerManager().ClearTimer(PeriodTimerHandle);
}

FGAActiveEffectCue::~FGAActiveEffectCue()
{
	
}

void FGAActiveEffectCuesContainer::AddCue(const FGAActiveEffectCue& CueIn)
{
	ActiveCues.Add(CueIn);
	ActiveCues[ActiveCues.Num() - 1].OnActivated();
}

void FGAActiveEffectCuesContainer::CueRemoved(const FGAEffectHandle& HandleIn)
{
	for (auto It = ActiveCues.CreateIterator(); It; ++It)
	{
		if (It->MyHandle == HandleIn)
		{
			It->CueRemoved();
			ActiveCues.RemoveAt(It.GetIndex());
		}
	}
}
void FGAActiveEffectCuesContainer::CueExpired(const FGAEffectHandle& HandleIn)
{
	for (auto It = ActiveCues.CreateIterator(); It; ++It)
	{
		if (It->MyHandle == HandleIn)
		{
			It->CueExpired();
			ActiveCues.RemoveAt(It.GetIndex());
		}
	}
}