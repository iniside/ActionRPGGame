// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GameplayTagContainer.h"
#include "../GAAbilitiesComponent.h"
#include "../Attributes/GAAttributesBase.h"
#include "../IGAAbilities.h"

#include "../GAAbilitiesComponent.h"
#include "GAEffectExecution.h"
#include "GAEffectExtension.h"
#include "../GAGlobalTypes.h"
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
void FGAEffect::OnExecuted()
{
	OnEffectPeriod.ExecuteIfBound();
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
					ModInfoIn.Magnitude.DirectModifier.GetValue(), ModInfoIn.AttributeMod, Handle, Handle.GetEffectSpec()->AttributeTags);
			}
			case EGAMagnitudeCalculation::AttributeBased:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.AttributeBased.GetValue(Context), ModInfoIn.AttributeMod, Handle, Handle.GetEffectSpec()->AttributeTags);

			}
			case EGAMagnitudeCalculation::CurveBased:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.CurveBased.GetValue(Context), ModInfoIn.AttributeMod, Handle, Handle.GetEffectSpec()->AttributeTags);

			}
			case EGAMagnitudeCalculation::CustomCalculation:
			{
				return FGAEffectMod (ModInfoIn.Attribute, 
					ModInfoIn.Magnitude.Custom.GetValue(Handle), ModInfoIn.AttributeMod, Handle, Handle.GetEffectSpec()->AttributeTags);

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
		ApplyEffectWithPeriod(HandleIn);
		break;
	}
	case EGAEffectType::Duration:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply effect for duration"));
		ApplyEffectWithDuration(HandleIn);

		break;
	}
	case EGAEffectType::Infinite:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Apply periodic effect with infinite duration"));
		ApplyEffectWithDuration(HandleIn);
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
void FGAEffectContainer::ApplyEffectWithDuration(const FGAEffectHandle& InHandle)
{
	UGAGameEffectSpec* Spec = InHandle.GetEffectSpec();
	EGAEffectStacking Stacking = InHandle.GetEffectSpec()->Stacking;
	EAFAttributeStacking AttributeStacking = InHandle.GetAttributeStacking();
	switch (AttributeStacking)
	{
		case EAFAttributeStacking::Add:
		{
			//just add modifier
			AddEffectDuration(InHandle); 
			break;
		}
		case EAFAttributeStacking::Override:
		{
			if (CheckCanApplyDuration(InHandle))
				ApplyOverrideEffect(InHandle);
			break;
		}
		case EAFAttributeStacking::StrongerOverride:
		{
			if (CheckCanApplyDuration(InHandle))
				ApplyOverrideEffect(InHandle);
			break;
		}
	}
}
void FGAEffectContainer::ApplyEffectWithPeriod(const FGAEffectHandle& InHandle)
{
	UGAGameEffectSpec* Spec = InHandle.GetEffectSpec();
	EGAEffectStacking Stacking = InHandle.GetEffectSpec()->Stacking;

	switch (Stacking)
	{
	case EGAEffectStacking::Override:
	{
			ApplyOverrideEffect(InHandle);
		break;
	}
	case EGAEffectStacking::Duration:
	{
		TSet<FGAEffectHandle> Handles = GetHandlesByClass(InHandle);
		if (Handles.Num() > 0)
		{
			for (FGAEffectHandle& Handle : Handles)
			{
				ExtendEffectDuration(Handle, InHandle);
			}
		}
		else
		{
				AddEffectDuration(InHandle);
		}
		break;
	}
	case EGAEffectStacking::Add:
	{
			AddEffectDuration(InHandle);
		break;
	}
	default:
		break;
	}
}
bool FGAEffectContainer::CheckCanApplyDuration(const FGAEffectHandle& InHandle)
{
	FGAEffectMod mod = InHandle.GetAttributeModifier();
	FGAAttribute attribute = InHandle.GetAttribute();
	FAFAttributeBase* attr = InHandle.GetContext().TargetComp->GetAttribute(attribute);
	if (attr && attr->Stacking == EAFAttributeStacking::StrongerOverride)
	{
		bool result = attr->CheckIfStronger(InHandle, mod);
		if (result)
		{
			TSet<FGAEffectHandle> handles = GetHandlesByClass(InHandle);
			for (const FGAEffectHandle& handle : handles)
			{
				RemoveEffect(handle);
			}
			RemoveEffect(InHandle);
		}
		return result;
	}
	if (attr && attr->Stacking == EAFAttributeStacking::Override)
	{
		TSet<FGAEffectHandle> handles = GetHandlesByClass(InHandle);
		for (const FGAEffectHandle& handle : handles)
		{
			RemoveEffect(handle);
		}
		RemoveEffect(InHandle);
	}
	return true;
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
EGAEffectAggregation FGAEffectContainer::GetEffectAggregation(const FGAEffectHandle& HandleIn) const
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	return Spec->EffectAggregation;
}

void FGAEffectContainer::AddEffectDuration(const FGAEffectHandle& HandleIn)
{
	FGAEffect& ExtEffect = HandleIn.GetEffectRef();
	if (ExtEffect.GameEffect->EffectType == EGAEffectType::Duration
		|| ExtEffect.GameEffect->EffectType == EGAEffectType::Periodic)
	{
		FTimerManager& DurationTimer = HandleIn.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, HandleIn);
		DurationTimer.SetTimer(ExtEffect.DurationTimerHandle, delDuration,
			ExtEffect.GetDurationTime(), false);
	}
	if (ExtEffect.GameEffect->EffectType == EGAEffectType::Periodic)
	{
		FTimerManager& PeriodTimer = HandleIn.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExecuteEffect, HandleIn);
		PeriodTimer.SetTimer(ExtEffect.PeriodTimerHandle, PeriodDuration,
			ExtEffect.GetPeriodTime(), true);
	}
	if(ExtEffect.GameEffect->bExecuteOnApplication)
		HandleIn.GetContext().TargetComp->ExecuteEffect(HandleIn);

	AddEffect(HandleIn);
}

void FGAEffectContainer::ExtendEffectDuration(const FGAEffectHandle& ExistingEffect, const FGAEffectHandle& HandleIn)
{
	FGAEffect& ExtEffect = ExistingEffect.GetEffectRef();
	FGAEffect& Effect = HandleIn.GetEffectRef();
	FTimerManager& DurationTimer = ExistingEffect.GetContext().TargetComp->GetWorld()->GetTimerManager();
	float RemainingTime = DurationTimer.GetTimerRemaining(ExtEffect.DurationTimerHandle);
	float NewDuration = RemainingTime + Effect.GetDurationTime();
	DurationTimer.ClearTimer(ExtEffect.DurationTimerHandle);

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(ExistingEffect.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, ExistingEffect);
	DurationTimer.SetTimer(ExtEffect.DurationTimerHandle, delDuration,
		NewDuration, false);
}

void FGAEffectContainer::ApplyOverrideEffect(const FGAEffectHandle& HandleIn)
{
	//it should account for all effects of the same class.
	TSet<FGAEffectHandle> handles = GetHandlesByClass(HandleIn);
	for (const FGAEffectHandle& handle : handles)
	{
		RemoveEffect(handle);
	}
	handles = GetHandlesByAttribute(HandleIn);
	for (const FGAEffectHandle& handle : handles)
	{
		RemoveEffect(handle);
	}
	RemoveEffect(HandleIn);
	AddEffectDuration(HandleIn);
}

TSet<FGAEffectHandle> FGAEffectContainer::GetHandlesByAttribute(const FGAEffectHandle& HandleIn)
{
	TSet<FGAEffectHandle> Handles;
	if (TSet<FGAEffectHandle>* ptr = EffectByAttribute.Find(HandleIn.GetAttribute()))
	{
		Handles = *ptr;
	}
	return Handles;
}
TSet<FGAEffectHandle> FGAEffectContainer::GetHandlesByClass(const FGAEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	UClass* EffectClass = Spec->GetClass();
	TSet<FGAEffectHandle> Handles;

	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		UGAAbilitiesComponent* Instigator = HandleIn.GetContextRef().InstigatorComp.Get();
		TMap<UClass*, TSet<FGAEffectHandle>>* EffectByClassMap = InstigatorEffectByClass.Find(Instigator);
		//TSet<FGAEffectHandle>* EffectByClass;
		if (EffectByClassMap)
		{
			return EffectByClassMap->FindRef(EffectClass);
		}

		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
		TSet<FGAEffectHandle>* EffectByClass = TargetEffectByClass.Find(EffectClass);
		if (EffectByClass)
		{
			return *EffectByClass;
		}
		break;
	}
	default:
		break;
	}
	return Handles;
}
void FGAEffectContainer::AddEffect(const FGAEffectHandle& HandleIn)
{
	TSet<FGAEffectHandle>& AttributeEffect = EffectByAttribute.FindOrAdd(HandleIn.GetAttribute());
	AttributeEffect.Add(HandleIn);
	ActiveEffectHandles.Add(HandleIn);
	AddEffectByClass(HandleIn);
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	if (Spec->EffectType == EGAEffectType::Infinite)
	{
		InfiniteEffects.Add(HandleIn);
	}
}
void FGAEffectContainer::AddEffectByClass(const FGAEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	UClass* EffectClass = Spec->GetClass();
	TSet<FGAEffectHandle> Handles;
	UGAAbilitiesComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		UGAAbilitiesComponent* Instigator = HandleIn.GetContextRef().InstigatorComp.Get();

		
		TMap<UClass*, TSet<FGAEffectHandle>>& EffectByClassMap = InstigatorEffectByClass.FindOrAdd(Instigator);
		TSet<FGAEffectHandle>& EffectByClass = EffectByClassMap.FindOrAdd(EffectClass);
		EffectByClass.Add(HandleIn);
		if (Target)
		{
			Target->AddTagContainer(Spec->ApplyTags);
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{		
		TSet<FGAEffectHandle>& EffectByClass = TargetEffectByClass.FindOrAdd(EffectClass);
		EffectByClass.Add(HandleIn);
		if (Target)
		{
			Target->AddTagContainer(Spec->ApplyTags);
		}
		break;
	}
	default:
		break;
	}
}
void FGAEffectContainer::RemoveFromAttribute(const FGAEffectHandle& HandleIn)
{
	TSet<FGAEffectHandle>* AttributeEffect = EffectByAttribute.Find(HandleIn.GetAttribute());
	IIGAAbilities* Target = HandleIn.GetContextRef().TargetInterface;
	if (AttributeEffect)
	{
		AttributeEffect->Remove(HandleIn);
		if (AttributeEffect->Num() <= 0)
		{
			EffectByAttribute.Remove(HandleIn.GetAttribute());
		}
	}

	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::RemoveFromAttribute %s = %f"), *HandleIn.GetAttribute().ToString(), Target->GetAttributeValue(HandleIn.GetAttribute()));
	Target->RemoveBonus(HandleIn.GetAttribute(), HandleIn, HandleIn.GetAttributeMod());
	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::RemoveFromAttribute %s = %f"), *HandleIn.GetAttribute().ToString(), Target->GetAttributeValue(HandleIn.GetAttribute()));
}
void FGAEffectContainer::RemoveEffectProtected(const FGAEffectHandle& HandleIn)
{
	RemoveFromAttribute(HandleIn);
	ActiveEffectHandles.Remove(HandleIn);
	InfiniteEffects.Remove(HandleIn);
}
void FGAEffectContainer::RemoveInstigatorEffect(const FGAEffectHandle& HandleIn)
{
	UGAAbilitiesComponent* Instigator = HandleIn.GetContextRef().InstigatorComp.Get();
	UClass* EffectClass = HandleIn.GetEffectSpec()->GetClass();
	TMap<UClass*, TSet<FGAEffectHandle>>* EffectByClass = InstigatorEffectByClass.Find(Instigator);
	TSet<FGAEffectHandle>* HandlesToRemove = EffectByClass->Find(EffectClass);

	if (HandlesToRemove)
	{
		HandlesToRemove->Remove(HandleIn);
		if (HandlesToRemove->Num() <= 0)
		{
			EffectByClass->Remove(EffectClass);
		}
	}
	RemoveEffectProtected(HandleIn);
	TSharedPtr<FGAEffect> Effect = HandleIn.GetEffectPtr();
	IIGAAbilities* Target = HandleIn.GetContextRef().TargetInterface;
	if (Effect.IsValid())
	{
		Effect->OnEffectRemoved.ExecuteIfBound();
		Target->RemoveTagContainer(Effect->ApplyTags);
		FTimerManager& DurationTimer = HandleIn.GetContext().TargetComp->GetWorld()->GetTimerManager();
		DurationTimer.ClearTimer(Effect->DurationTimerHandle);
		DurationTimer.ClearTimer(Effect->PeriodTimerHandle);
	}
}
void FGAEffectContainer::RemoveTargetEffect(const FGAEffectHandle& HandleIn)
{
	UClass* EffectClass = HandleIn.GetEffectSpec()->GetClass();
	TSet<FGAEffectHandle>* Handles = TargetEffectByClass.Find(EffectClass);
	IIGAAbilities* Target = HandleIn.GetContextRef().TargetInterface;
	
	TSharedPtr<FGAEffect> Effect = HandleIn.GetEffectPtr();

	if (Handles)
	{
		Handles->Remove(HandleIn);
		if (Handles->Num() <= 0)
		{
			TargetEffectByClass.Remove(EffectClass);
		}
	}

	RemoveEffectProtected(HandleIn);
	if (Effect.IsValid())
	{
		Effect->OnEffectRemoved.ExecuteIfBound();
		Target->RemoveTagContainer(Effect->ApplyTags);
	}
}

void FGAEffectContainer::RemoveEffect(const FGAEffectHandle& HandleIn, int32 Num)
{
	if (!ActiveEffectHandles.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
		return;
	}
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	UClass* EffectClass = Spec->GetClass();

	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		RemoveInstigatorEffect(HandleIn);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		RemoveTargetEffect(HandleIn);
		break;
	}
	default:
		break;
	}
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
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		break;
	}
	}
}


bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn)
{
	if (ActiveEffectHandles.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
//TSharedPtr<FGAEffect> FGAEffectContainer::GetEffectByHandle(const FGAEffectHandle& HandleIn)
//{
//	return ActiveEffects.FindRef(HandleIn);
//}
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