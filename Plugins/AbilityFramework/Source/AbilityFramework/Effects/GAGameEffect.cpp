// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GameplayTagContainer.h"
#include "../AFAbilityComponent.h"
#include "../Attributes/GAAttributesBase.h"
#include "../AFAbilityInterface.h"

#include "../AFAbilityComponent.h"
#include "GAEffectExecution.h"
#include "GAEffectExtension.h"
#include "../GAGlobalTypes.h"
#include "AFEffectApplicationRequirement.h"
#include "AFEffectCustomApplication.h"
#include "GAGameEffect.h"

DEFINE_STAT(STAT_GatherModifiers);

void FGAEffectProperty::Initialize()
{
	if (SpecClass.SpecClass)
	{
		Spec = SpecClass.SpecClass->GetDefaultObject<UGAGameEffectSpec>();
		ApplicationRequirement = GetSpec()->ApplicationRequirement.GetDefaultObject();
		Application = GetSpec()->Application.GetDefaultObject();
		Execution = GetSpec()->ExecutionType.GetDefaultObject();
	}
}
void FGAEffectProperty::InitializeIfNotInitialized()
{
	if (!IsInitialized())
	{
		Initialize();
	}
}
FAFEffectRepInfo::FAFEffectRepInfo(float AppliedTimeIn, float PeriodTimeIn, float DurationIn, float ReplicationTimeIn,
	class UAFAbilityComponent* InComponent)
	: AppliedTime(AppliedTimeIn),
	PeriodTime(PeriodTimeIn),
	Duration(DurationIn),
	ReplicationTime(ReplicationTimeIn),
	OwningComoponent(InComponent)
{

};
void FAFEffectRepInfo::Init()
{
	FTimerManager& Timer = OwningComoponent->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateRaw(this, &FAFEffectRepInfo::OnExpired);
	Timer.SetTimer(ExpiredHandle, delDuration,
		Duration, false);

	FTimerDelegate PeriodDuration = FTimerDelegate::CreateRaw(this, &FAFEffectRepInfo::OnPeriod);
	Timer.SetTimer(PeriodHandle, PeriodDuration,
		PeriodTime, true);
}
void FAFEffectRepInfo::OnExpired()
{
	OwningComoponent->ExecuteEffectEvent(OnExpiredEvent);
}
void FAFEffectRepInfo::OnPeriod()
{
	OwningComoponent->ExecuteEffectEvent(OnPeriodEvent);
}
void FAFEffectRepInfo::OnRemoved()
{
	FTimerManager& Timer = OwningComoponent->GetWorld()->GetTimerManager();
	Timer.ClearTimer(ExpiredHandle);
	Timer.ClearTimer(PeriodHandle);
	OwningComoponent->RemoveEffectEvent(OnExpiredEvent);
	OwningComoponent->RemoveEffectEvent(OnPeriodEvent);
	OwningComoponent->RemoveEffectEvent(OnRemovedEvent);
}

void FAFEffectRepInfo::PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer)
{
	InArraySerializer.EffectInfos.Remove(Handle);
	//InArraySerializer.OwningComponent->OnEffectRepInfoRemoved.Broadcast(this);
	FTimerManager& Timer = OwningComoponent->GetWorld()->GetTimerManager();
	Timer.ClearTimer(ExpiredHandle);
	Timer.ClearTimer(PeriodHandle);
	OwningComoponent->RemoveEffectEvent(OnExpiredEvent);
	OwningComoponent->RemoveEffectEvent(OnPeriodEvent);
	OwningComoponent->RemoveEffectEvent(OnRemovedEvent);
}
void FAFEffectRepInfo::PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer)
{
	OwningComoponent = InArraySerializer.OwningComponent;
	InArraySerializer.EffectInfos.Add(Handle, this);
	InArraySerializer.OwningComponent->OnEffectRepInfoApplied.Broadcast(this);

	//OwningComoponent->ExecuteEffectEvent(OnAppliedEvent);

	FTimerManager& Timer = OwningComoponent->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateRaw(this, &FAFEffectRepInfo::OnExpired);
	Timer.SetTimer(ExpiredHandle, delDuration,
		Duration, false);

	FTimerDelegate PeriodDuration = FTimerDelegate::CreateRaw(this, &FAFEffectRepInfo::OnPeriod);
	Timer.SetTimer(PeriodHandle, PeriodDuration,
		PeriodTime, true);

}
void FAFEffectRepInfo::PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer)
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
	Context(ContextIn)
{
	OwnedTags = GameEffectIn->OwnedTags;
	if (GameEffect->Extension)
	{
		Extension = NewObject<UGAEffectExtension>(Context.Target.Get(), GameEffect->Extension);
		Extension->OwningComponent = Context.TargetComp.Get();
	}
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

float FAFStatics::GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn
	, const FGAEffectContext& InContext
	, const FGAEffectHandle& InHandle)
{
	switch (AttributeIn.CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
	{
		return AttributeIn.DirectModifier.GetValue();
	}
	case EGAMagnitudeCalculation::AttributeBased:
	{
		return AttributeIn.AttributeBased.GetValue(InContext);
	}
	case EGAMagnitudeCalculation::CurveBased:
	{
		return AttributeIn.CurveBased.GetValue(InContext);
	}
	case EGAMagnitudeCalculation::CustomCalculation:
	{
		return AttributeIn.Custom.GetValue(InHandle);
	}
	default:
		break;
	}

	return 0;
}
FGAEffectMod FAFStatics::GetAttributeModifier(FGAAttributeModifier& ModInfoIn
	, class UGAGameEffectSpec* InSpec
	, const FGAEffectContext& InContext
	, const FGAEffectHandle& InHandle)
{
	FGAEffectMod ModOut;
	if (InSpec)
	{
		switch (ModInfoIn.Magnitude.CalculationType)
		{
		case EGAMagnitudeCalculation::Direct:
		{
			return FGAEffectMod(ModInfoIn.Attribute,
				ModInfoIn.Magnitude.DirectModifier.GetValue(), ModInfoIn.AttributeMod, InHandle, InSpec->AttributeTags);
		}
		case EGAMagnitudeCalculation::AttributeBased:
		{
			return FGAEffectMod(ModInfoIn.Attribute,
				ModInfoIn.Magnitude.AttributeBased.GetValue(InContext), ModInfoIn.AttributeMod, InHandle, InSpec->AttributeTags);

		}
		case EGAMagnitudeCalculation::CurveBased:
		{
			return FGAEffectMod(ModInfoIn.Attribute,
				ModInfoIn.Magnitude.CurveBased.GetValue(InContext), ModInfoIn.AttributeMod, InHandle, InSpec->AttributeTags);

		}
		case EGAMagnitudeCalculation::CustomCalculation:
		{
			return FGAEffectMod(ModInfoIn.Attribute,
				ModInfoIn.Magnitude.Custom.GetValue(InHandle), ModInfoIn.AttributeMod, InHandle, InSpec->AttributeTags);

		}
		default:
			break;
		}
	}
	return ModOut;
}
FGAEffect::~FGAEffect()
{
	if (Extension.IsValid())
	{
		Extension->MarkPendingKill();
		Extension.Reset();
	}
}
void FGAEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}

void FGAEffect::OnApplied()
{
	IsActive = true;
	AppliedTime = TargetWorld->TimeSeconds;
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectApplied();
	}
}
void FGAEffect::OnDuration()
{
	
}
void FGAEffect::OnExecuted()
{
	OnEffectPeriod.Broadcast(Handle);
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectExecuted();
	}
}
void FGAEffect::OnExpired()
{
	OnEffectExpired.Broadcast(Handle);
	IsActive = false;
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectExpired();
	}
}
void FGAEffect::OnRemoved()
{
	OnEffectRemoved.Broadcast(Handle);
	IsActive = false;
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectRemoved();
	}
}
void FGAEffect::DurationExpired()
{

}

float FGAEffect::GetDurationTime() const
{
	return GetFloatFromAttributeMagnitude(GameEffect->Duration);
}
float FGAEffect::GetPeriodTime() const
{
	return GetFloatFromAttributeMagnitude(GameEffect->Period);
}
float FGAEffect::GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn) const
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

FGAEffectHandle FGAEffectContainer::ApplyEffect(FGAEffect* EffectIn, FGAEffectProperty& InProperty
	, const FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
	FGAEffectHandle Handle;
	bool bHasDuration = InProperty.Duration > 0;
	bool bHasPeriod = InProperty.Period > 0;

	//we should not generate handle on clients.
	if (bHasDuration || bHasPeriod)
	{
		Handle = FGAEffectHandle::GenerateHandle(EffectIn);
	}
	if (InProperty.ApplicationRequirement->CanApply(EffectIn, InProperty, this, InContext, Handle))
	{
		if(!bHasDuration && !bHasPeriod)
		{
			if (InProperty.Handle.IsValid())
			{
				if (InProperty.Application->ApplyEffect(InProperty.Handle,
					EffectIn, InProperty, this, InContext))
				{
					Handle = InProperty.Handle;
					InProperty.Application->ExecuteEffect(InProperty.Handle, InProperty, InContext, Modifier);
					//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
				}
			}
			else
			{
				Handle = FGAEffectHandle::GenerateHandle(EffectIn);
				EffectIn->Handle = Handle;
				InProperty.Handle = Handle;
				if (InProperty.Application->ApplyEffect(Handle,
					EffectIn, InProperty, this, InContext))
				{
					InProperty.Application->ExecuteEffect(Handle, InProperty, InContext, Modifier);
					//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
				}
			}
			
		}
		else
		{
			EffectIn->Handle = Handle;
			if (InProperty.Application->ApplyEffect(Handle,
				EffectIn, InProperty, this, InContext))
			{
				InProperty.Application->ExecuteEffect(Handle, InProperty, InContext, Modifier);
				ApplyReplicationInfo(Handle, InProperty);
				//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
			}
			
		}
		
	}
	EffectIn->OnApplied();
	return Handle;
	//apply additonal effect applied with this effect.
	//for (TSubclassOf<UGAGameEffectSpec> Spec : EffectIn.GameEffect->OnAppliedEffects)
	//{
	//	FGAEffectHandle Handle = EffectIn.Context.TargetComp->MakeGameEffect(Spec, EffectIn.Context);

	//	EffectIn.Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle, InProperty);
	//}
}
void FGAEffectContainer::ApplyReplicationInfo(const FGAEffectHandle& InHandle, const FGAEffectProperty& InProperty)
{
	ENetMode mode = OwningComponent->GetNetMode();
	ENetRole role = OwningComponent->GetOwnerRole();
	if (mode == ENetMode::NM_DedicatedServer)
	{
		const UWorld* World = OwningComponent->GetWorld();
		FAFEffectRepInfo* RepInfo = new FAFEffectRepInfo(World->GetTimeSeconds(), InProperty.Period, InProperty.Duration, 0, OwningComponent);
		RepInfo->OnExpiredEvent = InProperty.GetSpec()->OnExpiredEvent;
		RepInfo->OnPeriodEvent = InProperty.GetSpec()->OnPeriodEvent;
		RepInfo->OnRemovedEvent = InProperty.GetSpec()->OnRemovedEvent;
		RepInfo->Handle = InHandle;
		RepInfo->Init();
		MarkItemDirty(*RepInfo);
		ActiveEffectInfos.Add(*RepInfo);
		MarkArrayDirty();
		
		//predictevily add effect info ?
		//if (mode == ENetMode::NM_Standalone
		//	|| role == ENetRole::ROLE_Authority)
		{
			EffectInfos.Add(InHandle, RepInfo);
		}
	}
	else if(mode == ENetMode::NM_Standalone)
	{
		
		const UWorld* World = OwningComponent->GetWorld();
		FAFEffectRepInfo* RepInfo = new FAFEffectRepInfo(World->GetTimeSeconds(), InProperty.Period, InProperty.Duration, 0, OwningComponent);
		RepInfo->OnExpiredEvent = InProperty.GetSpec()->OnExpiredEvent;
		RepInfo->OnPeriodEvent = InProperty.GetSpec()->OnPeriodEvent;
		RepInfo->OnRemovedEvent = InProperty.GetSpec()->OnRemovedEvent;
		RepInfo->Handle = InHandle;
		RepInfo->Init();
		//MarkItemDirty(RepInfo);
		ActiveEffectInfos.Add(*RepInfo);
		//MarkArrayDirty();

		//predictevily add effect info ?
		{
			EffectInfos.Add(InHandle, RepInfo);
		}
	}
}

EGAEffectAggregation FGAEffectContainer::GetEffectAggregation(const FGAEffectHandle& HandleIn) const
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	return Spec->EffectAggregation;
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

TSet<FGAEffectHandle> FGAEffectContainer::GetHandlesByClass(const FGAEffectProperty& InProperty
, const FGAEffectContext& InContext)
{
	TSet<FGAEffectHandle> Handles;
	UGAGameEffectSpec* Spec = InProperty.Spec;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	UClass* EffectClass = Spec->GetClass();

	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		UAFAbilityComponent* Instigator = InContext.InstigatorComp.Get();
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
		UAFAbilityComponent* Target = InContext.TargetComp.Get();
		TSet<FGAEffectHandle>* TargetEffect = TargetEffectByClass.Find(EffectClass);
		if (TargetEffect)
		{
			return *TargetEffect;
		}
		break;
	}
	default:
		break;
	}
	return Handles;
}

void FGAEffectContainer::AddEffect(const FGAEffectHandle& HandleIn, bool bInfinite)
{
	TSet<FGAEffectHandle>& AttributeEffect = EffectByAttribute.FindOrAdd(HandleIn.GetAttribute());
	AttributeEffect.Add(HandleIn);
	ActiveEffectHandles.Add(HandleIn);
	AddEffectByClass(HandleIn);
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	FObjectKey key(HandleIn.GetEffectSpec()->GetClass());
	TArray<FGAEffectHandle>& handles = EffectByClass.FindOrAdd(key);
	handles.Add(HandleIn);
	if (bInfinite)
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
	UAFAbilityComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		UAFAbilityComponent* Instigator = HandleIn.GetContextRef().InstigatorComp.Get();

		
		TMap<UClass*, TSet<FGAEffectHandle>>& EffectByClassMap = InstigatorEffectByClass.FindOrAdd(Instigator);
		TSet<FGAEffectHandle>& instigatorEffect = EffectByClassMap.FindOrAdd(EffectClass);
		instigatorEffect.Add(HandleIn);
		if (Target)
		{
			Target->AddTagContainer(Spec->ApplyTags);
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{		
		TSet<FGAEffectHandle>& TargetEffects = TargetEffectByClass.FindOrAdd(EffectClass);
		TargetEffects.Add(HandleIn);
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
	IAFAbilityInterface* Target = HandleIn.GetContextRef().TargetInterface;
	if (AttributeEffect)
	{
		AttributeEffect->Remove(HandleIn);
		if (AttributeEffect->Num() <= 0)
		{
			EffectByAttribute.Remove(HandleIn.GetAttribute());
		}
	}

	//UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::RemoveFromAttribute %s = %f"), *HandleIn.GetAttribute().ToString(), Target->GetAttributeValue(HandleIn.GetAttribute()));
	Target->RemoveBonus(HandleIn.GetAttribute(), HandleIn, HandleIn.GetAttributeMod());
	//UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::RemoveFromAttribute %s = %f"), *HandleIn.GetAttribute().ToString(), Target->GetAttributeValue(HandleIn.GetAttribute()));
}
void FGAEffectContainer::RemoveEffectProtected(const FGAEffectHandle& HandleIn
	, const FGAEffectProperty& InProperty)
{
	RemoveFromAttribute(HandleIn);
	ActiveEffectHandles.Remove(HandleIn);
	InfiniteEffects.Remove(HandleIn);
	TArray<FGAEffectHandle>* handles = EffectByClass.Find(FObjectKey(InProperty.GetClass()));
	if (handles && handles->Num() > 0)
	{
		handles->Remove(HandleIn);
		if (handles->Num() <= 0)
		{
			EffectByClass.Remove(FObjectKey(InProperty.GetClass()));
		}
	}
}
void FGAEffectContainer::RemoveInstigatorEffect(const FGAEffectHandle& HandleIn
	, const FGAEffectProperty& InProperty)
{
	UAFAbilityComponent* Instigator = HandleIn.GetContextRef().InstigatorComp.Get();
	UClass* EffectClass = HandleIn.GetEffectSpec()->GetClass();
	TMap<UClass*, TSet<FGAEffectHandle>>* InstigatorEffect = InstigatorEffectByClass.Find(Instigator);
	TSet<FGAEffectHandle>* HandlesToRemove = InstigatorEffect->Find(EffectClass);
	IAFAbilityInterface* Target = HandleIn.GetContextRef().TargetInterface;
	TSharedPtr<FGAEffect> Effect = HandleIn.GetEffectPtr();
	if (HandlesToRemove)
	{
		HandlesToRemove->Remove(HandleIn);
		if (HandlesToRemove->Num() <= 0)
		{
			InstigatorEffect->Remove(EffectClass);
		}
	}
	if (Effect.IsValid())
	{
		Effect->OnEffectRemoved.Broadcast(Effect->Handle);
		Target->RemoveTagContainer(Effect->ApplyTags);
		FTimerManager& DurationTimer = Effect->Context.TargetComp->GetWorld()->GetTimerManager();
		DurationTimer.ClearTimer(Effect->DurationTimerHandle);
		DurationTimer.ClearTimer(Effect->PeriodTimerHandle);
	}
	RemoveEffectProtected(HandleIn, InProperty);

}

void FGAEffectContainer::RemoveEffectByHandle(const FGAEffectHandle& InHandle, const FGAEffectProperty& InProperty)
{
	UGAGameEffectSpec* Spec = InProperty.Spec;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	//TSet<FGAEffectHandle>* handles = EffectByClass.Find(FObjectKey(HandleIn.GetClass()));//GetHandlesByClass(HandleIn, InContext);

	if (!ActiveEffectHandles.Contains(InHandle))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *InHandle.GetEffectRef().ToString());
		return;
	}
	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		RemoveInstigatorEffect(InHandle, InProperty);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		RemoveTargetEffect(InHandle, InProperty);
		break;
	}
	default:
		break;
	}
}

void FGAEffectContainer::RemoveTargetEffect(const FGAEffectHandle& HandleIn
	, const FGAEffectProperty& InProperty)
{
	UClass* EffectClass = HandleIn.GetEffectSpec()->GetClass();
	TSet<FGAEffectHandle>* Handles = TargetEffectByClass.Find(EffectClass);
	IAFAbilityInterface* Target = HandleIn.GetContextRef().TargetInterface;
	
	TSharedPtr<FGAEffect> Effect = HandleIn.GetEffectPtr();

	if (Handles)
	{
		Handles->Remove(HandleIn);
		if (Handles->Num() <= 0)
		{
			TargetEffectByClass.Remove(EffectClass);
		}
	}

	RemoveEffectProtected(HandleIn, InProperty);
	if (Effect.IsValid())
	{
		Effect->OnEffectRemoved.Broadcast(Effect->Handle);
		Target->RemoveTagContainer(Effect->ApplyTags);
		FTimerManager& DurationTimer = Effect->Context.TargetComp->GetWorld()->GetTimerManager();
		DurationTimer.ClearTimer(Effect->DurationTimerHandle);
		DurationTimer.ClearTimer(Effect->PeriodTimerHandle);
	}
}

void FGAEffectContainer::RemoveEffect(const FGAEffectProperty& HandleIn, int32 Num)
{
	UGAGameEffectSpec* Spec = HandleIn.Spec;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	TArray<FGAEffectHandle>* handles = EffectByClass.Find(FObjectKey(HandleIn.GetClass()));//GetHandlesByClass(HandleIn, InContext);
	FAFEffectRepInfo* Out = nullptr;
	

	if (handles)
	{
		for (int32 idx = 0; idx < Num; idx++)
		{
			if (handles->IsValidIndex(0))
			{
				FGAEffectHandle OutHandle = (*handles)[0];
				if (OutHandle.IsValid())
				{
					EffectInfos.RemoveAndCopyValue(OutHandle, Out);
					if (Out)
					{
						MarkItemDirty(*Out);
						Out->OnRemoved();
						ActiveEffectInfos.Remove(*Out);
						MarkArrayDirty();
						delete Out;
					}
					if (!ActiveEffectHandles.Contains(OutHandle))
					{
						UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *OutHandle.GetEffectRef().ToString());
						continue;
					}
					switch (Aggregation)
					{
					case EGAEffectAggregation::AggregateByInstigator:
					{
						RemoveInstigatorEffect(OutHandle, HandleIn);
						break;
					}
					case EGAEffectAggregation::AggregateByTarget:
					{
						RemoveTargetEffect(OutHandle, HandleIn);
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}
}

//FGAEffectContainer::FGAEffectContainer()
//{
//}

void FGAEffectContainer::ApplyEffectInstance(class UGAEffectExtension* EffectIn)
{

}


bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn)
{
	if (ActiveEffectHandles.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
bool FGAEffectContainer::ContainsEffectOfClass(const FGAEffectProperty& InProperty)
{
	FObjectKey key(InProperty.GetClass());
	if (EffectByClass.Contains(key))
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
	ApplicationRequirement = UAFEffectApplicationRequirement::StaticClass();
	Application = UAFEffectCustomApplication::StaticClass();
}