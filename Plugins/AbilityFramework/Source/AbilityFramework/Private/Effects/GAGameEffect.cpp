// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "GameplayTagContainer.h"
#include "AFEffectsComponent.h"
#include "Attributes/GAAttributesBase.h"
#include "AFAbilityInterface.h"

#include "GAEffectExecution.h"
#include "GAEffectExtension.h"
#include "GAGlobalTypes.h"

#include "AFEffectApplicationRequirement.h"
#include "AFEffectCustomApplication.h"
#include "GAGameEffect.h"
#include "GABlueprintLibrary.h"

DEFINE_STAT(STAT_GatherModifiers);


void FAFEffectSpec::OnApplied() const
{
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectApplied();
	}
}
void FAFEffectSpec::OnExpired() const
{
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectExpired();
	}
}
void FAFEffectSpec::OnRemoved() const
{
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectRemoved();
	}
}
void FAFEffectSpec::OnExecuted() const
{
	if (Extension.IsValid())
	{
		Extension->NativeOnEffectExecuted();
	}
}
FAFEffectSpec::FAFEffectSpec(const FAFContextHandle& InContext, TSubclassOf<UGAGameEffectSpec> InSpecClass)
	: SpecClass(InSpecClass)
{
	Context = InContext;
	Extension = nullptr;
	SpecClass = InSpecClass;
	OwnedTags.AppendTags(InSpecClass.GetDefaultObject()->OwnedTags);
}
void FAFEffectSpec::Instance()
{
	if (SpecClass.GetDefaultObject()->Extension)
	{
		Extension = NewObject<UGAEffectExtension>(Context.GetPtr()->Target.Get(), SpecClass.GetDefaultObject()->Extension);
		Extension->OwningComponent = Context.GetPtr()->TargetInterface->GetEffectsComponent();
		Extension->SourceComponent = Context.GetPtr()->InstigatorInterface->GetEffectsComponent();
	}
}
float FAFEffectSpec::GetFloatFromAttributeMagnitude(
	  const FGAMagnitude& AttributeIn
	, const FGAEffectContext& InContext) const
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
		return AttributeIn.Custom.GetValue(InContext);
	}
	default:
		break;
	}

	return 0;
}

void FAFEffectSpec::OverrideAttributeModifier(float InValue)
{
	EffectMod.Value = InValue;
}
void FAFEffectSpec::CalculateAttributeModifier(const FGAEffectHandle& InHandle)
{
	FGAEffectContext& InContext = Context.GetRef();
	EffectMod = FAFStatics::GetAttributeModifier(SpecClass.GetDefaultObject()->AtributeModifier
		, SpecClass.GetDefaultObject()
		, InContext
		, InHandle);
}

float FAFEffectSpec::GetDuration(const FGAEffectContext& InContext)
{
	return GetFloatFromAttributeMagnitude(GetSpec()->Duration, InContext);
}
float FAFEffectSpec::GetPeriod(const FGAEffectContext& InContext)
{
	return GetFloatFromAttributeMagnitude(GetSpec()->Period, InContext);
}

float FAFEffectSpec::GetDuration(const FGAEffectContext& InContext) const
{
	return GetFloatFromAttributeMagnitude(GetSpec()->Duration, InContext);
}
float FAFEffectSpec::GetPeriod(const FGAEffectContext& InContext) const
{
	return GetFloatFromAttributeMagnitude(GetSpec()->Period, InContext);
}

FGAEffectProperty::FGAEffectProperty()
	: ApplicationRequirement(nullptr)
	, Application(nullptr)
	, Execution(nullptr)
	, Spec(nullptr)
	, Duration(0)
	, Period(0)
	, bInstant(true)
{
	ApplicationRequirement = nullptr;
	Application = nullptr;
	Execution = nullptr;
	Spec = nullptr;
	Duration= 0;
	Period = 0;
};
FGAEffectProperty::FGAEffectProperty(TSubclassOf<UGAGameEffectSpec> InClass)
	: ApplicationRequirement(nullptr)
	, Application(nullptr)
	, Execution(nullptr)
	, Spec(nullptr)
	, Duration(0)
	, Period(0)
	, bInstant(true)
{
};
void FGAEffectProperty::PostScriptConstruct()
{
	ApplicationRequirement = nullptr;
	Application = nullptr;
	Execution = nullptr;
	Spec = nullptr;
	Duration = 0;
	Period = 0;
}
void FGAEffectProperty::Initialize(TSubclassOf<UGAGameEffectSpec> EffectClass)
{
	if (EffectClass)
	{
		Spec = EffectClass->GetDefaultObject<UGAGameEffectSpec>();
		ApplicationRequirement = GetSpecData()->ApplicationRequirement.GetDefaultObject();
		Application = GetSpecData()->Application.GetDefaultObject();
		Execution = GetSpecData()->ExecutionType.GetDefaultObject();
	}
}
void FGAEffectProperty::InitializeIfNotInitialized(
	class APawn* Instigator
	, UObject* Causer
	, TSubclassOf<UGAGameEffectSpec> EffectClass
	)
{
	if (!IsInitialized())
	{
		SpecClass = EffectClass;

		FGAEffectContext* Context = new FGAEffectContext(Instigator, Causer);

		EffectContext = FAFContextHandle::Generate(Context);
		Initialize(EffectClass);
	}
	if (Spec.IsValid())
	{
		Duration = GetSpecData()->Duration.GetFloatValue(EffectContext.GetRef());
		Period = GetSpecData()->Period.GetFloatValue(EffectContext.GetRef());

		if ((Duration > 0) || (Period > 0))
		{
			bInstant = false;
		}
	}
}
bool FGAEffectProperty::CanApply(
	const struct FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& InParams
	, const FGAEffectHandle& InHandle)
{
	return ApplicationRequirement->CanApply(EffectIn, InParams, InHandle, InContainer);
}
bool FGAEffectProperty::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const struct FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& InParams
	, const FAFFunctionModifier& Modifier)
{
	return Application->ApplyEffect(InHandle, EffectIn, InContainer, InParams, Modifier);
}
void FGAEffectProperty::ApplyExecute(const FGAEffectHandle& InHandle
	, const FAFEffectParams& InParams
	, const FAFFunctionModifier& Modifier)
{
	Application->ApplyExecute(InHandle, InParams, Modifier);
}

void FGAEffectProperty::ExecuteEffect(
	const FGAEffectHandle& HandleIn
	, FGAEffectMod& ModIn
	, const FAFEffectParams& InParams
	, const FAFFunctionModifier& Modifier)
{
	Execution->ExecuteEffect(HandleIn, ModIn, InParams, Modifier);
}

void FGAEffect::PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer)
{
	//const_cast<FGAEffectContainer&>(InArraySerializer).RemoveFromReplication(Handle, PredictionHandle);
}
void FGAEffect::PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer)
{
	World = InArraySerializer.OwningComponent->GetWorld();
	AppliedTime = World->TimeSeconds;
	LastTickTime = World->TimeSeconds;
	
	
	Handle = FGAEffectHandle::GenerateHandle();

	//const_cast<FGAEffectContainer&>(InArraySerializer).ApplyFromReplication(Handle, PredictionHandle);
}
void FGAEffect::PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer)
{

}
FGAEffect::FGAEffect(TSharedPtr<FAFEffectSpec> InSpec, const FGAEffectHandle& InHandle)
{
	Handle = InHandle;
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
		return Custom.GetValue(Context);
	}
	default:
		return 0;
	}

	return 0;
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
		return AttributeIn.Custom.GetValue(InContext);
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
				ModInfoIn.Magnitude.Custom.GetValue(InContext), ModInfoIn.AttributeMod, InHandle, InSpec->AttributeTags);

		}
		default:
			break;
		}
	}
	return ModOut;
}

FGAEffect::~FGAEffect()
{
}


float FGAEffect::GetDurationTime() const
{
	return Duration;// GetFloatFromAttributeMagnitude(GetEffect()->Duration);
}
float FGAEffect::GetPeriodTime() const
{
	return Period;// GetFloatFromAttributeMagnitude(GetEffect()->Period);
}

float FGAEffect::GetCurrentDuration() const
{
	if (World)
	{
		float CurrentTime = World->TimeSeconds;
		return CurrentTime - AppliedTime;
	}
	
	return 0;
}
FTimerManager& FAFEffectParams::GetTargetTimerManager()
{
	return Context.TargetComp->GetWorld()->GetTimerManager();
}
UAFEffectsComponent* FAFEffectParams::GetTargetEffectsComponent()
{
	return Context.GetTargetEffectsComponent();
}
UAFEffectsComponent* FAFEffectParams::GetTargetEffectsComponent() const
{
	return Context.GetTargetEffectsComponent();
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

FGAEffectHandle FGAEffectContainer::ApplyEffect(
	const FGAEffect& EffectIn
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	FGAEffectHandle Handle;
	FGAEffectProperty& InProperty = Params.GetProperty();

	bool bHasDuration = InProperty.GetDuration() > 0;
	bool bHasPeriod = InProperty.GetPeriod() > 0;
	
	//InProperty.DataTest->ID++;

	ENetRole role = OwningComponent->GetOwnerRole();
	ENetMode mode = OwningComponent->GetOwner()->GetNetMode();
	
	Handle = FGAEffectHandle::GenerateHandle();

	if (InProperty.CanApply(EffectIn, this, Params, Handle))
	{
		if(!Params.bPeriodicEffect) //instatnt effect.
		{
			const_cast<FGAEffect&>(EffectIn).SetHandle(Handle);
			if (InProperty.ApplyEffect(Handle,
				EffectIn, this, Params))
			{
				InProperty.ApplyExecute(Handle, Params, Modifier);
				
				//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
			}	
		}
		else
		{
			const_cast<FGAEffect&>(EffectIn).SetHandle(Handle);
			if (InProperty.ApplyEffect(Handle,
				EffectIn, this, Params))
			{
				const FAFEffectSpec& Spec = Params.GetSpec();
				const FGAEffectContext& Context = Params.GetContext();
				
				const_cast<FGAEffect&>(EffectIn).AppliedTime = OwningComponent->GetWorld()->TimeSeconds;
				const_cast<FGAEffect&>(EffectIn).LastTickTime = OwningComponent->GetWorld()->TimeSeconds;
				const_cast<FGAEffect&>(EffectIn).Duration = Spec.GetDuration(Context);
				const_cast<FGAEffect&>(EffectIn).Period = Spec.GetPeriod(Context);
				MarkItemDirty(const_cast<FGAEffect&>(EffectIn));
				int32 newItem = ActiveEffectInfos.Add(EffectIn);
				MarkArrayDirty();
				AddEffect(Handle, const_cast<FGAEffect&>(EffectIn).PredictionHandle, &ActiveEffectInfos[newItem], InProperty, Params);
				
				//InProperty.ApplyExecute(Handle, Params, Modifier);
				//generate it only on client, and apply prediction key from client.
				//if server replicates with valid key, then nothing happens.
				//if not we try to rewind effect application.
				//we probabaly don't need to unwind attribute changes, since next replication from
				//server will overridem them anyway.
				
				bool bIsServer = GEngine->GetNetMode(OwningComponent->GetWorld()) == ENetMode::NM_DedicatedServer;
			}
		}
		
	}
	//right place ?
	Params.GetSpec().OnApplied();
	
	FAFEventData EventData;
	const FGameplayTagContainer& AppliedEvents = InProperty.GetSpecData()->AppliedEventTags;
	for(const FGameplayTag& Event : AppliedEvents)
		OwningComponent->TriggerAppliedEvent(Event, EventData);

	const FGAEffectContext& InContext = Params.GetContext();
	TArray<FAFConditionalEffect>& Effects = InProperty.GetSpecData()->IfHaveTagEffect.Effects;
	for (FAFConditionalEffect& Effect : Effects)
	{
		if (Effect.RequiredTag.IsValid()
			&& InContext.GetTargetEffectsComponent()->HasTag(Effect.RequiredTag))
		{
			//Hack. We need a way store handles for conditional effects.
			FAFPropertytHandle PropertyNew(Effect.Effect);
			FGAEffectHandle OtherHandle;
			/*OtherHandle = UGABlueprintLibrary::ApplyEffect(PropertyNew
				, OtherHandle
				, InContext.Target.Get()
				, InContext.Instigator.Get()
				, InContext.Causer.Get()
				, InContext.HitResult);*/
		}
	}
	

	return Handle;
	
}

TSet<FGAEffectHandle> FGAEffectContainer::GetHandlesByClass(const FGAEffectProperty& InProperty
, const FGAEffectContext& InContext)
{
	TSet<FGAEffectHandle> Handles;
	UGAGameEffectSpec* Spec = InProperty.GetSpecData();
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

void FGAEffectContainer::AddEffect(
	const FGAEffectHandle& InHandle
	, const FAFPredictionHandle& InPredHandle
	, FGAEffect* InEffect
	, const FGAEffectProperty& InProperty
	, const FAFEffectParams& Params
	, bool bInfinite)
{
	HandleByPrediction.Add(InPredHandle, InHandle);
	PredictionByHandle.Add(InHandle, InPredHandle);
	PredictedEffectInfos.Add(InPredHandle, InEffect);
	UGAGameEffectSpec* Spec = InProperty.GetSpecData();
	UClass* SpecClass = Spec->GetClass();
	const FGAEffectContext& Context = Params.GetContext();

	TSet<FGAEffectHandle>& Effects = EffectByAttribute.FindOrAdd(Spec->AtributeModifier.Attribute);
	Effects.Add(InHandle);

	FObjectKey EffectKey(SpecClass);
	TArray<FGAEffectHandle>& LocalEffectClass = EffectByClass.FindOrAdd(EffectKey);
	LocalEffectClass.Add(InHandle);

	switch (Spec->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<UClass*, TSet<FGAEffectHandle>>& InstEffectClass = InstigatorEffectByClass.FindOrAdd(Context.Instigator.Get());
		TSet<FGAEffectHandle>& EffectClass2 = InstEffectClass.FindOrAdd(SpecClass);
		EffectClass2.Add(InHandle);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		TSet<FGAEffectHandle>& EffectClass = TargetEffectByClass.FindOrAdd(SpecClass);
		EffectClass.Add(InHandle);
		break;
	}
	}

	ActiveEffects.Add(InHandle, InEffect);
	FGAEffectContainer* sss = this;
	int dbg = 0;
}

void FGAEffectContainer::RemoveEffectByHandle(const FGAEffectHandle& InHandle, const FGAEffectContext& InContext, const FAFPropertytHandle& InProperty)
{
	RemoveEffectInternal(InProperty, InContext, InHandle);
}

TArray<FGAEffectHandle> FGAEffectContainer::RemoveEffect(const FAFPropertytHandle& HandleIn, const FGAEffectContext& InContext, int32 Num)
{
	UGAGameEffectSpec* Spec = HandleIn.GetSpecData();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	FObjectKey key(HandleIn.GetClass());
	TArray<FGAEffectHandle>* handles = EffectByClass.Find(key);//GetHandlesByClass(HandleIn, InContext);
	FGAEffectContainer* sss = this;
	if (!handles)
		return TArray<FGAEffectHandle>();
	TArray<FGAEffectHandle> copy = *handles;

	for (int32 idx = 0; idx < Num; idx++)
	{
		if (handles->IsValidIndex(0))
		{
			FGAEffectHandle OutHandle = (*handles)[idx];
			RemoveEffectInternal(HandleIn, InContext, OutHandle);
		}
	}

	

	return copy;
}

bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn)
{
	if (ActiveEffects.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn) const
{
	if (ActiveEffects.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
bool FGAEffectContainer::ContainsEffectOfClass(const FAFPropertytHandle& InProperty)
{
	FObjectKey key(InProperty.GetClass());
	if (EffectByClass.Contains(key))
	{
		return true;
	}
	return false;
}

void FGAEffectContainer::ApplyFromReplication(const FGAEffectHandle& InHandle, const FAFPredictionHandle& InPredHandle, FGAEffect* InEffect)
{
	//AddEffect(InHandle, InPredHandle, InEffect);
}
void FGAEffectContainer::RemoveFromReplication(const FGAEffectHandle& InHandle, const FAFPredictionHandle& InPredHandle)
{
	//FGAEffect* Effect = ActiveEffects[InHandle];
	//APawn* Instigator = Effect->GetContext().Instigator.Get();
	//UObject* Target = Effect->GetContext().Target.Get();

	//HandleByPrediction.Remove(InPredHandle);
	//PredictionByHandle.Remove(InHandle);
	//PredictedEffectInfos.Remove(InPredHandle);

	//FGAAttribute Attribute = Effect->GetEffect()->AtributeModifier.Attribute;

	//TSet<FGAEffectHandle>* Effects = EffectByAttribute.Find(Attribute);
	//if (Effects)
	//{
	//	Effects->Remove(InHandle);
	//	if (Effects->Num() == 0)
	//	{
	//		EffectByAttribute.Remove(Attribute);
	//	}
	//}

	//FObjectKey EffectKey(Effect->GameEffectClass);
	//TArray<FGAEffectHandle>* EffectClass = EffectByClass.Find(EffectKey);
	//if (EffectClass)
	//{
	//	EffectClass->Remove(InHandle);
	//	if (EffectClass->Num() == 0)
	//	{
	//		EffectByClass.Remove(EffectKey);
	//	}
	//}

	//switch (Effect->GetEffect()->EffectAggregation)
	//{
	//case EGAEffectAggregation::AggregateByInstigator:
	//{
	//	TMap<UClass*, TSet<FGAEffectHandle>>* InstEffectClass = InstigatorEffectByClass.Find(Instigator);
	//	if (InstEffectClass)
	//	{
	//		TSet<FGAEffectHandle>* EffectClass2 = InstEffectClass->Find(Effect->GameEffectClass);
	//		if (EffectClass2)
	//		{
	//			EffectClass2->Remove(InHandle);
	//			if (EffectClass2->Num() == 0)
	//			{
	//				InstEffectClass->Remove(Effect->GameEffectClass);
	//			}
	//		}
	//		if (InstEffectClass->Num() == 0)
	//		{
	//			InstigatorEffectByClass.Remove(Instigator);
	//		}
	//	}
	//	
	//	break;
	//}
	//case EGAEffectAggregation::AggregateByTarget:
	//{
	//	TSet<FGAEffectHandle>* EffectClass2 = TargetEffectByClass.Find(Effect->GameEffectClass);
	//	if (EffectClass2)
	//	{
	//		EffectClass2->Remove(InHandle);
	//		if (EffectClass2->Num() == 0)
	//		{
	//			TargetEffectByClass.Remove(Effect->GameEffectClass);
	//		}
	//	}
	//	break;
	//}
	//}
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
	ApplicationRequirement = UAFEffectApplicationRequirement::StaticClass();
	Application = UAFEffectCustomApplication::StaticClass();
}

float FGAEffectContainer::GetRemainingTime(const FGAEffectHandle& InHandle) const
{
	const FGAEffect* const * Effect = ActiveEffects.Find(InHandle);
	if (Effect)
	{
		const FGAEffect* Ptr = *Effect;
		float Duration = Ptr->GetDurationTime();
		return FMath::Clamp<float>(Duration - Ptr->GetCurrentDuration(), 0, Duration);
	}
	return 0;
}
float FGAEffectContainer::GetRemainingTimeNormalized(const FGAEffectHandle& InHandle) const
{
	const FGAEffect* const * Effect = ActiveEffects.Find(InHandle);
	if (Effect)
	{
		const FGAEffect* Ptr = *Effect;
		float CurrentDuration = Ptr->GetCurrentDuration();
		float MaxDuration = Ptr->GetDurationTime();

		float CurrentTime = ((CurrentDuration / MaxDuration) - 1) * (-1);

		return CurrentTime;// FMath::Clamp<float>(CurrentTime, 1, 0);
	}
	return 0;
}
float FGAEffectContainer::GetCurrentTime(const FGAEffectHandle& InHandle) const
{
	const FGAEffect* const * Effect = ActiveEffects.Find(InHandle);
	if (Effect)
	{
		const FGAEffect* Ptr = *Effect;
		return Ptr->GetCurrentDuration();
	}
	return 0;
}
float FGAEffectContainer::GetCurrentTimeNormalized(const FGAEffectHandle& InHandle) const
{
	const FGAEffect* const * Effect = ActiveEffects.Find(InHandle);
	if (Effect)
	{
		const FGAEffect* Ptr = *Effect;
		float CurrentDuration = Ptr->GetCurrentDuration();
		float MaxDuration = Ptr->GetDurationTime();
		return CurrentDuration * 1 / MaxDuration;
	}
	return 0;
}
float FGAEffectContainer::GetEndTime(const FGAEffectHandle& InHandle) const
{
	return 0;
}

void FGAEffectContainer::RemoveEffectInternal(const FAFPropertytHandle& InProperty, const FGAEffectContext& InContext, const FGAEffectHandle& InHandle)
{
	FGAEffect* Effect = ActiveEffects[InHandle];
	UGAGameEffectSpec* Spec = InProperty.GetSpecData();
	UClass* SpecClass = Spec->GetClass();

	FTimerManager& DurationTimer = InContext.TargetComp->GetWorld()->GetTimerManager();
	DurationTimer.ClearTimer(Effect->DurationTimerHandle);
	DurationTimer.ClearTimer(Effect->PeriodTimerHandle);

	APawn* Instigator = InContext.Instigator.Get();
	UObject* ObjectTarget = InContext.Target.Get();
	FAFPredictionHandle PredHandle = PredictionByHandle[InHandle];

	HandleByPrediction.Remove(PredHandle);
	PredictionByHandle.Remove(InHandle);
	PredictedEffectInfos.Remove(PredHandle);

	FGAAttribute Attribute = Spec->AtributeModifier.Attribute;
	EGAAttributeMod AttributeMod = Spec->AtributeModifier.AttributeMod;;

	TSet<FGAEffectHandle>* Effects = EffectByAttribute.Find(Spec->AtributeModifier.Attribute);
	if (Effects)
	{
		IAFAbilityInterface* IntTarget = InContext.TargetInterface;
		IntTarget->RemoveBonus(Attribute, InHandle, AttributeMod);
		Effects->Remove(InHandle);
		if (Effects->Num() == 0)
		{
			EffectByAttribute.Remove(Spec->AtributeModifier.Attribute);
		}
	}

	FObjectKey EffectKey(SpecClass);
	TArray<FGAEffectHandle>* EffectClass = EffectByClass.Find(EffectKey);
	if (EffectClass)
	{
		EffectClass->Remove(InHandle);
		if (EffectClass->Num() == 0)
		{
			EffectByClass.Remove(EffectKey);
		}
	}

	switch (Spec->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<UClass*, TSet<FGAEffectHandle>>* InstEffectClass = InstigatorEffectByClass.Find(Instigator);
		if (InstEffectClass)
		{
			TSet<FGAEffectHandle>* EffectClass2 = InstEffectClass->Find(SpecClass);
			if (EffectClass2)
			{
				EffectClass2->Remove(InHandle);
				if (EffectClass2->Num() == 0)
				{
					InstEffectClass->Remove(SpecClass);
				}
			}
			if (InstEffectClass->Num() == 0)
			{
				InstigatorEffectByClass.Remove(Instigator);
			}
		}

		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		TSet<FGAEffectHandle>* EffectClass2 = TargetEffectByClass.Find(SpecClass);
		if (EffectClass2)
		{
			EffectClass2->Remove(InHandle);
			if (EffectClass2->Num() == 0)
			{
				TargetEffectByClass.Remove(SpecClass);
			}
		}
		break;
	}
	}
	ActiveEffects.Remove(InHandle);
}

bool FGAEffectContainer::IsEffectActive(TSubclassOf<UGAGameEffectSpec> EffectClass)
{
	FObjectKey EffectKey(EffectClass);
	if (EffectByClass.Contains(EffectKey))
	{
		return true;
	}
	return false;
}

void FAFEffectContainerSimple::ApplyEffect(const FGAEffectHandle& InHandle
	, const FGAEffect& InEffect)
{
	Effects.Add(InHandle, InEffect);
}
void FAFEffectContainerSimple::RemoveEffect(const FGAEffectHandle& InHandle)
{
	Effects.Remove(InHandle);
}