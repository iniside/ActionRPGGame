// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAttributes.h"
#include "GAEffectCue.h"

UGAEffectCue::UGAEffectCue()
{
	bEffectRemoved = false;
	bTickEnabled = false;
}
UWorld* UGAEffectCue::GetWorld() const
{
	if (Owner)
	{
		return Owner->GetWorld();
	}
	return nullptr;
}

void UGAEffectCue::SetParameters(AActor* OwnerIn, const FGAEffectCueParams& CueParamsIn)
{
	CueParams = CueParamsIn;
	Owner = OwnerIn;
}

void UGAEffectCue::BeginCue()
{
	if (bEnableTick)
	{
		bTickEnabled = true;
	}
	if (GetWorld())
	{
		OnEffectApplied();
		if (CueParams.Duration > 0)
		{
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			
			FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UGAEffectCue::OnEffectEnded);
			TimerManager.SetTimer(DurationTimerHandle, EndDelegate, CueParams.Duration, false);
			
			if (CueParams.Period > 0)
			{
				FTimerDelegate PeriodDelegate = FTimerDelegate::CreateUObject(this, &UGAEffectCue::OnEffectPeriod);
				TimerManager.SetTimer(PeriodTimerHandle, PeriodDelegate, CueParams.Period, true);
			}
		}
	}
}
void UGAEffectCue::ExecuteCue()
{
	OnEffectPeriod();
}
void UGAEffectCue::UpdateDuration(float NewDurationIn)
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(DurationTimerHandle);

		FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UGAEffectCue::OnEffectEnded);
		TimerManager.SetTimer(DurationTimerHandle, EndDelegate, NewDurationIn, false);
	}
}
void UGAEffectCue::ExtendDuration(float Duration)
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		float RemaningDuration = TimerManager.GetTimerRemaining(DurationTimerHandle);
		float NewDuration = Duration + RemaningDuration;
		TimerManager.ClearTimer(DurationTimerHandle);

		FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UGAEffectCue::OnEffectEnded);
		TimerManager.SetTimer(DurationTimerHandle, EndDelegate, NewDuration, false);
	}
}
void UGAEffectCue::UpdatePeriod(float NewPeriodIn)
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(PeriodTimerHandle);

		FTimerDelegate PeriodDelegate = FTimerDelegate::CreateUObject(this, &UGAEffectCue::OnEffectPeriod);
		TimerManager.SetTimer(PeriodTimerHandle, PeriodDelegate, NewPeriodIn, true);
	}
}
void UGAEffectCue::UpdateTimers(float NewDurationIn, float NewPeriodIn)
{
	UpdateDuration(NewDurationIn);
	UpdatePeriod(NewPeriodIn);
}
void UGAEffectCue::EndCue()
{
	bTickEnabled = false;
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(DurationTimerHandle);
		TimerManager.ClearTimer(PeriodTimerHandle);
	}
	OnEffectRemoved();
}
void UGAEffectCue::Tick(float DeltaTime)
{
	OnTick();
}
bool UGAEffectCue::IsTickable() const
{
	return bTickEnabled;
}
TStatId UGAEffectCue::GetStatId() const
{
	return GetStatID();
}

bool UGAEffectCue::IsTickableWhenPaused() const
{
	return false;
}

bool UGAEffectCue::IsTickableInEditor() const
{
	return false;
}