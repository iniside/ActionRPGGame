// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "GameplayTagContainer.h"
#include "GAAbilitiesComponent.h"
#include "GAAttributesBase.h"
#include "IGAAbilities.h"

#include "GAAbilitiesComponent.h"
#include "GAEffectExecution.h"
#include "GAEffectExtension.h"
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"

DEFINE_STAT(STAT_GatherModifiers);

void FGAEffectRepInfo::OnApplied()
{
	/*UGAEffectCue* NewCue = NewObject<UGAEffectCue>(EffectCueClass);
	if (NewCue)
	{
		if (Context.TargetComp.IsValid())
		{
			NewCue->Owner = Context.Target.Get();
			NewCue->Context = Context;
			OnAppliedDelegate.CreateUObject(NewCue, &UGAEffectCue::OnEffectApplied);
			Context.TargetComp->GameEffectContainer.EffectCues.Add(Handle, NewCue);
			OnAppliedDelegate.ExecuteIfBound();
		}
	}*/
}
void FGAEffectRepInfo::OnPeriod()
{

}
void FGAEffectRepInfo::OnRemoved()
{

}

void FGAEffectRepInfo::PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer)
{

}
void FGAEffectRepInfo::PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer)
{
		OnApplied();
}
void FGAEffectRepInfo::PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer)
{

}
float FGAMagnitude::GetFloatValue(const FGAEffectContext& Context)
{
	switch (CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
	{
		return DirectModifier.GetValue();
	}
	case EGAMagnitudeCalculation::AttributeBased:
	{
		return AttributeBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::SummedAttributeBased:
	{
		return SummedAttributeBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::CurveBased:
	{
		return CurveBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::CustomCalculation:
	{
		//return  Custom.GetValue(*this);
	}
	default:
		return 0;
	}

	return 0;
}
FGAEffect::FGAEffect(class UGAGameEffectSpec* GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffect(GameEffectIn),
	Context(ContextIn),
	Execution(GameEffect->ExecutionType.GetDefaultObject())
{
	OwnedTags = GameEffectIn->OwnedTags;
	if (ContextIn.TargetComp.IsValid())
	{
		TargetWorld = ContextIn.TargetComp->GetWorld();
		AppliedTime = TargetWorld->TimeSeconds;
		LastTickTime = TargetWorld->TimeSeconds;
	}
	else if (ContextIn.InstigatorComp.IsValid())
	{
		TargetWorld = ContextIn.InstigatorComp->GetWorld();
		AppliedTime = TargetWorld->TimeSeconds;
		LastTickTime = TargetWorld->TimeSeconds;
	}
	IsActive = false;
}
FGAEffect::~FGAEffect()
{
}
void FGAEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}

FGAEffectMod FGAEffect::GetAttributeModifier()
{
	return GetMod(GameEffect->AtributeModifier);
}
void FGAEffect::OnApplied()
{
	IsActive = true;
	AppliedTime = TargetWorld->TimeSeconds;
}
void FGAEffect::OnDuration()
{

}
void FGAEffect::OnExpired()
{
	OnEffectExpired.ExecuteIfBound();
	IsActive = false;
}
void FGAEffect::OnRemoved()
{
	OnEffectRemoved.ExecuteIfBound();
	IsActive = false;
}
void FGAEffect::DurationExpired()
{

}

float FGAEffect::GetDurationTime()
{
	return GetFloatFromAttributeMagnitude(GameEffect->Duration);
}
float FGAEffect::GetPeriodTime()
{
	return GetFloatFromAttributeMagnitude(GameEffect->Period);
}
float FGAEffect::GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn)
{
	switch (AttributeIn.CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
	{
		return AttributeIn.DirectModifier.GetValue();
	}
	case EGAMagnitudeCalculation::AttributeBased:
	{
		return AttributeIn.AttributeBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::SummedAttributeBased:
	{
		return AttributeIn.SummedAttributeBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::CurveBased:
	{
		return AttributeIn.CurveBased.GetValue(Context);
	}
	case EGAMagnitudeCalculation::CustomCalculation:
	{
		return AttributeIn.Custom.GetValue(Handle);
	}
	default:
		break;
	}

	return 0;
}
float FGAEffect::GetCurrentActivationTime()
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - AppliedTime;
}

float FGAEffect::GetCurrentActivationTime() const
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - AppliedTime;
}

float FGAEffect::GetCurrentTickTime()
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - LastTickTime;
}

FGAEffectMod FGAEffect::GetMod(FGAAttributeModifier& ModInfoIn)
{
	FGAEffectMod ModOut;
	if (GameEffect)
	{
			switch (ModInfoIn.Magnitude.CalculationType)
			{
			case EGAMagnitudeCalculation::Direct:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.DirectModifier.GetValue(), ModInfoIn.AttributeMod, Handle);
			}
			case EGAMagnitudeCalculation::AttributeBased:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.AttributeBased.GetValue(Context), ModInfoIn.AttributeMod, Handle);

			}
			case EGAMagnitudeCalculation::SummedAttributeBased:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.SummedAttributeBased.GetValue(Context), ModInfoIn.AttributeMod, Handle);

			}
			case EGAMagnitudeCalculation::CurveBased:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.CurveBased.GetValue(Context), ModInfoIn.AttributeMod, Handle);

			}
			case EGAMagnitudeCalculation::CustomCalculation:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.Custom.GetValue(Handle), ModInfoIn.AttributeMod, Handle);

			}
			default:
				break;
			}
	}
	return ModOut;
}

void FGameCueContainer::AddCue(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams)
{
	/*if (!EffectCue)
	{
		return;
	}
	AActor* Instigator = EffectHandle.GetContext().Instigator.Get();
	UGAEffectCue* NewCue = NewObject<UGAEffectCue>(Instigator, EffectCue);
	if (NewCue && Instigator)
	{
		NewCue->SetParameters(Instigator, CueParams);
		NewCue->Context = EffectHandle.GetContext();
		NewCue->BeginCue();
	}*/
}

void FGAEffectContainer::ApplyEffect(const FGAEffect& EffectIn
	, const FGAEffectHandle& HandleIn)
{
	//instant effect goes trough simplest application path
	//just make effect and run it trough modifiers.

	//FGAEffect Effect = EfNoConst.Calculation->ModiifyEffect(EffectIn);
	UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply periodic effect: %s"), *EffectIn.GameEffect->GetName());
	switch (EffectIn.GameEffect->EffectType)
	{
	case EGAEffectType::Instant:
	{
		//dont do anything fancy now simply execute effect and forget about it.
		HandleIn.GetContext().TargetComp->ExecuteEffect(HandleIn);
		break;
	}
	case EGAEffectType::Periodic:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply periodic effect"));
		InternalCheckPeriodicEffectStacking(HandleIn);
		break;
	}
	case EGAEffectType::Duration:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply effect for duration"));
		InternalCheckDurationEffectStacking(HandleIn);
		break;
	}
	case EGAEffectType::Infinite:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply periodic effect with infinite duration"));
		InternalCheckInfiniteEffectStacking(HandleIn);
		break;
	}
	default:
		break;
	}
	HandleIn.GetEffectRef().OnApplied();
	ApplyReplicationInfo(HandleIn);
	//apply additonal effect applied with this effect.
	for (TSubclassOf<UGAGameEffectSpec> Spec : EffectIn.GameEffect->OnAppliedEffects)
	{
		FGAEffectHandle Handle = EffectIn.Context.TargetComp->MakeGameEffect(Spec, EffectIn.Context);

		EffectIn.Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle);
	}
}
void FGAEffectContainer::ApplyReplicationInfo(const FGAEffectHandle& HandleIn)
{
	//TSharedPtr<FGAEffect> EffectPtr = HandleIn.GetEffectPtr();
	//UGAGameEffectSpec* EffectSpec = HandleIn.GetEffectSpec();
	//if (EffectPtr.IsValid() && EffectSpec && EffectSpec->EffectCue)
	//{
	//	FGAEffectRepInfo RepInfo(0, EffectSpec->Period, EffectSpec->Duration, 0);
	////	RepInfo.EffectCueClass = EffectSpec->EffectCue;
	//	MarkItemDirty(RepInfo);
	//}
}
void FGAEffectContainer::InternalCheckDurationEffectStacking(const FGAEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	UE_LOG(GameAttributes, Log, TEXT("Stacking Type: %s"), *EnumToString::GetStatckingAsString(Stacking));
	FGAEffectHandle* Handle = nullptr;
	switch (Aggregation)
	{
		case EGAEffectAggregation::AggregateByInstigator:
		{
			TMap<FName, TSet<FGAEffectHandle>>* EffectHandle = InstigatorEffectHandles.Find(HandleIn.GetContextRef().Instigator.Get());
			TSet<FGAEffectHandle>* HandleSet = nullptr;
			if (EffectHandle)
			{
				HandleSet = EffectHandle->Find(HandleIn.GetEffectSpec()->GetFName());
				if (HandleSet)
				{
					Handle = HandleSet->Find(HandleIn);
				}
			}
		}
		case EGAEffectAggregation::AggregateByTarget:
		{
			Handle = TargetEffectByType.Find(HandleIn.GetEffectSpec()->GetFName());
			
			break;
		}
	}
	switch (Stacking)
	{
		case EGAEffectStacking::Add:
		{
			InternalApplyDuration(HandleIn);
			break;
		}
		case EGAEffectStacking::Duration:
		{
			if (Handle)
			{
				InternalExtendEffectDuration(*Handle, HandleIn);
			}
			else
			{
				FGAEffectHandle InvalidHandle;
				InternalExtendEffectDuration(HandleIn, InvalidHandle);
			}
			break;
		}
		case EGAEffectStacking::Intensity:
		{
			//Add new modifiers to existing effect without affecting old effect duration ?
			break;
		}
		case EGAEffectStacking::Override:
		{
			if (Handle)
			{
				//RemoveEffect(*Handle);
				RemoveOverrideEffects(*Handle);
			}
			else
			{
				RemoveOverrideEffects(HandleIn);
			}
			InternalApplyDuration(HandleIn);
			break;
		}
		case EGAEffectStacking::StrongerOverride:
		{
			//check which attributes are modified by effect, and then pick stronger one.
			//if (Handle)
			{
				//if (RemoveWeakerEffect(*Handle))
				{
					InternalApplyDuration(HandleIn);
				}
			}
			break;
		}
	}

	InternalApplyEffectByAggregation(HandleIn);
}
bool FGAEffectContainer::RemoveEffectsByTags(const FGameplayTagContainer& TagsIn)
{
	return false;
}
TSharedPtr<FGAEffect> FGAEffectContainer::RemoveEffectByAggregation(const FGAEffectHandle& HandleIn)
{
	EGAEffectAggregation aggregatiopn = HandleIn.GetEffectRef().GameEffect->EffectAggregation;
	UObject* Instigator = HandleIn.GetContextRef().Instigator.Get();
	UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	TSharedPtr<FGAEffect> Effect = ActiveEffects.FindRef(HandleIn);
	if (!Effect.IsValid())
		return nullptr;

	switch (aggregatiopn)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<FGAEffectHandle, TSharedPtr<FGAEffect>>* effects = InstigatorEffects.Find(Instigator);
		TMap<FName, TSet<FGAEffectHandle>>* EffectByClass = InstigatorEffectHandles.Find(Instigator);
		if (EffectByClass)
		{
			//Probabaly need another path for removing just single effect from stack.
			EffectByClass->Remove(HandleIn.GetEffectSpec()->GetFName());
		}
		if (effects)
		{
			effects->FindAndRemoveChecked(HandleIn);
			if (effects->Num() == 0)
			{
				InstigatorEffects.Remove(Instigator);
			}
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		//TargetEffects.FindAndRemoveChecked(HandleIn);
		//check aggregation type to know which effect to remove exactly ?
		TargetEffectByType.Remove(HandleIn.GetEffectSpec()->GetFName());
		break;
	}
	}
	Target->AppliedTags.RemoveTagContainer(Effect->ApplyTags);
	Effect->OnEffectRemoved.ExecuteIfBound();
	ActiveEffects.Remove(HandleIn);
	UE_LOG(GameAttributesEffects, Log, TEXT("FGAEffectContainer:: Removing Effect %s "), *Effect->GameEffect->GetName())
	return Effect;
}
bool FGAEffectContainer::RemoveWeakerEffect(const FGAEffectHandle& HandleIn)
{
	bool bRemoved = false;
	if (!ActiveEffects.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
		return bRemoved;
	}
	UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	
		FGAAttributeBase* attr = Target->GetAttribute(HandleIn.GetAttribute());
		if (attr)
		{
			if (attr->CheckIfStronger(HandleIn))
			{
				UE_LOG(GameAttributes, Log, TEXT("Value Before bonus of attribute = %f"), attr->GetFinalValue());
				attr->RemoveBonus(HandleIn);
				UE_LOG(GameAttributes, Log, TEXT("Value After bonus of attribute = %f"), attr->GetFinalValue());
				RemoveEffectByAggregation(HandleIn);
				bRemoved = true;
			}
		}
			//effect.Reset();
	
	return bRemoved;
}
void FGAEffectContainer::InternalCheckPeriodicEffectStacking(const FGAEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	UE_LOG(GameAttributes, Log, TEXT("Stacking Type: %s"), *EnumToString::GetStatckingAsString(Stacking));
	FGAEffectHandle* Handle = FindHandlePtrByAggregation(HandleIn);
	switch (Stacking)
	{
		case EGAEffectStacking::Add:
		{
			break;
		}
		case EGAEffectStacking::Duration:
		{
			break;
		}
		case EGAEffectStacking::Intensity:
		{
			break;
		}
		case EGAEffectStacking::Override:
		{
			break;
		}
		case EGAEffectStacking::StrongerOverride:
		{
			InternalApplyPeriodic(HandleIn);
			break;
		}
	}
	InternalApplyEffectByAggregation(HandleIn);
}
void FGAEffectContainer::InternalApplyPeriodic(const FGAEffectHandle& HandleIn)
{
	FGAEffect& EffectRef = HandleIn.GetEffectRef();
	FTimerDelegate delPeriod = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExecuteEffect, HandleIn);
	FTimerManager& PeriodicTimer = EffectRef.Context.TargetComp->GetWorld()->GetTimerManager();
	
	float period = EffectRef.GetPeriodTime();
	PeriodicTimer.SetTimer(EffectRef.PeriodTimerHandle, delPeriod,
		period, true, 0);

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = EffectRef.Context.TargetComp->GetWorld()->GetTimerManager();
	float Duration = EffectRef.GetDurationTime();
	timerDuration.SetTimer(EffectRef.DurationTimerHandle, delDuration,
		Duration, false, Duration);

	InternalApplyEffectTags(HandleIn);
}
void FGAEffectContainer::InternalApplyDuration(const FGAEffectHandle& HandleIn)
{
	FGAEffect& EffectRef = HandleIn.GetEffectRef();
	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = EffectRef.Context.TargetComp->GetWorld()->GetTimerManager();

	timerDuration.SetTimer(EffectRef.DurationTimerHandle, delDuration,
		EffectRef.GetDurationTime(), false, EffectRef.GetDurationTime());

	InternalApplyEffectTags(HandleIn);
	HandleIn.GetContext().TargetComp->ExecuteEffect(HandleIn);
}

void FGAEffectContainer::InternalCheckInfiniteEffectStacking(const FGAEffectHandle& HandleIn)
{
	InternalApplyInfiniteEffect(HandleIn);
	InternalApplyEffectByAggregation(HandleIn);
}
void FGAEffectContainer::InternalApplyInfiniteEffect(const FGAEffectHandle& HandleIn)
{
	FGAEffect& EffectRef = HandleIn.GetEffectRef();
	FTimerDelegate delPeriod = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExecuteEffect, HandleIn);
	FTimerManager& PeriodicTimer = EffectRef.Context.TargetComp->GetWorld()->GetTimerManager();
	float period = EffectRef.GetPeriodTime();
	PeriodicTimer.SetTimer(EffectRef.PeriodTimerHandle, delPeriod,
		period, true, 0);

	InternalApplyEffectTags(HandleIn);
	HandleIn.GetContext().TargetComp->ExecuteEffect(HandleIn);
}
void FGAEffectContainer::InternalApplyEffectTags(const FGAEffectHandle& HandleIn)
{
	//wayt to few dereferences!
	UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	FGAEffect& Effect = HandleIn.GetEffectRef();

	Target->AppliedTags.AddTagContainer(Effect.ApplyTags);
}
void FGAEffectContainer::InternalApplyEffectByAggregation(const FGAEffectHandle& HandleIn)
{
	EGAEffectAggregation Aggregation = HandleIn.GetEffectRef().GameEffect->EffectAggregation;

	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<FGAEffectHandle, TSharedPtr<FGAEffect>>& EffectMap = InstigatorEffects.FindOrAdd(HandleIn.GetContextRef().Instigator.Get());
		EffectMap.Add(HandleIn, HandleIn.GetEffectPtr());
		TMap<FName, TSet<FGAEffectHandle>>& EffectByClass = InstigatorEffectHandles.FindOrAdd(HandleIn.GetContextRef().Instigator.Get());
		TSet<FGAEffectHandle>& EffectSet = EffectByClass.FindOrAdd(HandleIn.GetEffectSpec()->GetFName());
		EffectSet.Add(HandleIn);

		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		//only one effect of given class can exist when aggregated by target.
		FGAEffectHandle& Handle = TargetEffectByType.FindOrAdd(HandleIn.GetEffectSpec()->GetFName());
		
		Handle = HandleIn;
		//TargetEffects.Add(HandleIn, HandleIn.GetEffectPtr());

		break;
	}
	}
	TSet<FGAEffectHandle>& Effects = EffectByAttribute.FindOrAdd(HandleIn.GetAttribute());
	Effects.Add(HandleIn);
	ActiveEffects.Add(HandleIn, HandleIn.GetEffectPtr());
}
void FGAEffectContainer::InternalApplyEffect(const FGAEffectHandle& HandleIn)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("InternalApplyEffect"));
	FGAEffect& Effect = HandleIn.GetEffectRef();
	if (HandleIn.GetEffectSpec()->EffectType == EGAEffectType::Periodic)
	{
		FTimerDelegate delPeriod = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExecuteEffect, HandleIn);
		FTimerManager& PeriodicTimer = Effect.Context.TargetComp->GetWorld()->GetTimerManager();

		PeriodicTimer.SetTimer(Effect.PeriodTimerHandle, delPeriod,
			Effect.GetPeriodTime(), true, 0);
	}
	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = Effect.Context.TargetComp->GetWorld()->GetTimerManager();

	timerDuration.SetTimer(Effect.DurationTimerHandle, delDuration,
		Effect.GetDurationTime(), false);

	HandleIn.GetEffectRef().OnApplied();
}
void FGAEffectContainer::InternalExtendEffectDuration(const FGAEffectHandle& HandleIn, const FGAEffectHandle& ExtendingHandleIn)
{
	if (ExtendingHandleIn.IsValid())
	{
		FGAEffect& ExtEffect = ExtendingHandleIn.GetEffectRef();
		FGAEffect& Effect = HandleIn.GetEffectRef();
		FTimerManager& DurationTimer = HandleIn.GetContext().TargetComp->GetWorld()->GetTimerManager();
		float RemainingTime = DurationTimer.GetTimerRemaining(Effect.DurationTimerHandle);
		float NewDuration = RemainingTime + ExtEffect.GetDurationTime();
		DurationTimer.ClearTimer(Effect.DurationTimerHandle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, HandleIn);
		DurationTimer.SetTimer(Effect.DurationTimerHandle, delDuration,
			NewDuration, false);
	}
	else
	{
		InternalApplyEffect(HandleIn);
	}
}
int32 FGAEffectContainer::RemoveOverrideEffects(const FGAEffectHandle& HandleIn)
{
	int32 EffectsRemoved = 0;
	if (!ActiveEffects.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveOverrideEffects Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
	//	return EffectsRemoved;
	}
	TSet<FGAEffectHandle> Effects = EffectByAttribute.FindRef(HandleIn.GetAttribute());
	TArray<FGAEffectHandle> EffectsToRemove;
	FGameplayTagQuery Q;
	
	for (const FGAEffectHandle& Handle : Effects)
	{
		Q.BuildQuery(FGameplayTagQueryExpression()
			.AllTagsMatch()
			.AddTags(Handle.GetOwnedTags())
		);
		bool queryMatch = HandleIn.GetOwnedTags().MatchesQuery(Q);
		if (Handle.HasAllTags(HandleIn.GetOwnedTags()) //add checking for number of tags ?
			&& Handle.GetAttributeMod() == HandleIn.GetAttributeMod())
		{
			//UE_LOG(GameAttributes, Log, TEXT("RemoveOverrideEffects Effect %s added to be removed, Tags: %s -- OtherTags: %s"), 
			//	*Handle.GetEffectRef().ToString(), *Handle.GetOwnedTags().ToString(), HandleIn.GetOwnedTags().ToString());
			EffectsToRemove.Add(Handle);
		}
	}
	for (FGAEffectHandle& HandleToRemove : EffectsToRemove)
	{
		RemoveEffect(HandleToRemove);
		EffectsRemoved++;
	}
	return EffectsRemoved;
}
int32 FGAEffectContainer::RemoveStrongerOverrideEffects(const FGAEffectHandle& HandleIn)
{
	int32 EffectsRemoved = 0;
	if (!ActiveEffects.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveOverrideEffects Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
		//	return EffectsRemoved;
	}
	TSet<FGAEffectHandle> Effects = EffectByAttribute.FindRef(HandleIn.GetAttribute());
	TArray<FGAEffectHandle> EffectsToRemove;
	for (const FGAEffectHandle& Handle : Effects)
	{
		if (Handle.HasAllTags(HandleIn.GetOwnedTags())
			&& HandleIn.GetAttributeModifier() > Handle.GetAttributeModifier())
		{
			//UE_LOG(GameAttributes, Log, TEXT("RemoveOverrideEffects Effect %s added to be removed, Tags: %s -- OtherTags: %s"), 
			//	*Handle.GetEffectRef().ToString(), *Handle.GetOwnedTags().ToString(), HandleIn.GetOwnedTags().ToString());
			EffectsToRemove.Add(Handle);
		}
	}
	for (FGAEffectHandle& HandleToRemove : EffectsToRemove)
	{
		RemoveEffect(HandleToRemove);
		EffectsRemoved++;
	}
	return EffectsRemoved;
}
void FGAEffectContainer::RemoveEffect(FGAEffectHandle& HandleIn)
{
	if (!ActiveEffects.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
		return;
	}
	EGAEffectAggregation aggregatiopn = HandleIn.GetEffectRef().GameEffect->EffectAggregation;
	UObject* Instigator = HandleIn.GetContextRef().Instigator.Get();
	UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	TSharedPtr<FGAEffect> effect = ActiveEffects.FindAndRemoveChecked(HandleIn);
	TSet<FGAEffectHandle> Effects = EffectByAttribute.FindRef(HandleIn.GetAttribute());
	
	//if (Effects)
	{
		Effects.Remove(HandleIn);
		if (Effects.Num() == 0)
		{
			EffectByAttribute.Remove(HandleIn.GetAttribute());
		}
	}
	if (effect.IsValid())
	{
		switch (aggregatiopn)
		{
			case EGAEffectAggregation::AggregateByInstigator:
			{
				TMap<FGAEffectHandle, TSharedPtr<FGAEffect>>* effects = InstigatorEffects.Find(Instigator);
				TMap<FName, TSet<FGAEffectHandle>>* EffectByClass = InstigatorEffectHandles.Find(Instigator);
				if (EffectByClass)
				{
					//Probabaly need another path for removing just single effect from stack.
					EffectByClass->Remove(HandleIn.GetEffectSpec()->GetFName());
				}
				if (effects)
				{
					effects->FindAndRemoveChecked(HandleIn);
					if (effects->Num() == 0)
					{
						InstigatorEffects.Remove(Instigator);
					}
				}
				break;
			}
			case EGAEffectAggregation::AggregateByTarget:
			{
				//TargetEffects.FindAndRemoveChecked(HandleIn);
				//check aggregation type to know which effect to remove exactly ?
				TargetEffectByType.Remove(HandleIn.GetEffectSpec()->GetFName());
				break;
			}
		}
		Target->AppliedTags.RemoveTagContainer(effect->ApplyTags);
		FGAAttributeBase* attr = Target->GetAttribute(HandleIn.GetEffectSpec()->AtributeModifier.Attribute);
		if (attr)
		{
			UE_LOG(GameAttributes, Log, TEXT("Value Before bonus of attribute = %f"), attr->GetFinalValue());
			attr->RemoveBonus(HandleIn);
			UE_LOG(GameAttributes, Log, TEXT("Value After bonus of attribute = %f"), attr->GetFinalValue());
		}
		effect->OnEffectRemoved.ExecuteIfBound();
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAEffectContainer:: Removing Effect %s "), *effect->GameEffect->GetName())
		//effect.Reset();
	}
}
FGAEffectHandle* FGAEffectContainer::FindHandlePtrByAggregation(const FGAEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	FGAEffectHandle* Handle = nullptr;
	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<FName, TSet<FGAEffectHandle>>* EffectHandle = InstigatorEffectHandles.Find(HandleIn.GetContextRef().Instigator.Get());
		TSet<FGAEffectHandle>* HandleSet = nullptr;
		if (EffectHandle)
		{
			HandleSet = EffectHandle->Find(HandleIn.GetEffectSpec()->GetFName());
			if (HandleSet)
			{
				Handle = HandleSet->Find(HandleIn);
			}
		}
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		Handle = TargetEffectByType.Find(HandleIn.GetEffectSpec()->GetFName());
		break;
	}
	}
	return Handle;
}
FGAEffectContainer::FGAEffectContainer()
{
}

void FGAEffectContainer::ApplyEffectInstance(class UGAEffectExtension* EffectIn)
{
	//do something ?
	switch (EffectIn->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		FGAInstigatorInstancedEffectContainer& Effects = InstigatorInstancedEffects.FindOrAdd(EffectIn->Context.Instigator.Get());
		Effects.Effects.AddUnique(EffectIn);
		//VERY BAD CHANGE IT.
		EffectIn->BeginEffect();
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		break;
	}
	}
}
void FGAEffectContainer::RemoveEffectInstance(class UGAEffectExtension* EffectIn)
{
	switch (EffectIn->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		FGAInstigatorInstancedEffectContainer* Effects = InstigatorInstancedEffects.Find(EffectIn->Context.Instigator.Get());
		if (Effects)
		{
			Effects->Effects.Remove(EffectIn);
			if (Effects->Effects.Num() == 0)
			{
				InstigatorEffects.Remove(EffectIn->Context.Instigator.Get());
			}
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		TargetInstancedEffects.Remove(EffectIn);
		break;
	}
	}
}

bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn)
{
	if (ActiveEffects.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
TSharedPtr<FGAEffect> FGAEffectContainer::GetEffectByHandle(const FGAEffectHandle& HandleIn)
{
	return ActiveEffects.FindRef(HandleIn);
}
UWorld* FGAEffectContainer::GetWorld() const
{
	if (OwningComponent)
	{
		return OwningComponent->GetWorld();
	}
	return nullptr;
}

UGAGameEffectSpec::UGAGameEffectSpec()
{
	ExecutionType = UGAEffectExecution::StaticClass();
}