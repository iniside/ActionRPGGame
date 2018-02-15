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
void FGAEffectProperty::InitializeIfNotInitialized(const FGAEffectContext& InContext)
{
	if (!IsInitialized())
	{
		Initialize();
	}
	if (Spec)
	{
		Duration = GetSpec()->Duration.GetFloatValue(InContext);
		Period = GetSpec()->Period.GetFloatValue(InContext);
	}
}
bool FGAEffectProperty::CanApply(FGAEffect* EffectIn
	, struct FGAEffectContainer* InContainer
	, const FGAEffectContext& InContext
	, const FGAEffectHandle& InHandle)
{
	return ApplicationRequirement->CanApply(EffectIn, *this, InContainer, InContext, InHandle);
}
bool FGAEffectProperty::ApplyEffect(const FGAEffectHandle& InHandle
	, struct FGAEffect* EffectIn
	, struct FGAEffectContainer* InContainer
	, const FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
	return Application->ApplyEffect(InHandle, EffectIn, *this, InContainer, InContext, Modifier);
}
void FGAEffectProperty::ExecuteEffect(const FGAEffectHandle& InHandle
	, const FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
	Application->ExecuteEffect(InHandle, *this, InContext, Modifier);
}

void FGAEffectProperty::EffectExecution(const FGAEffectHandle& HandleIn
	, FGAEffectMod& ModIn
	, FGAEffectContext& Context
	, const FAFFunctionModifier& Modifier)
{
	Execution->ExecuteEffect(HandleIn, ModIn, Context, *this, Modifier);
}

void FGAEffect::PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer)
{

}
void FGAEffect::PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer)
{
	AppliedTime = InArraySerializer.OwningComponent->GetWorld()->TimeSeconds;
	LastTickTime = InArraySerializer.OwningComponent->GetWorld()->TimeSeconds;
	Extension = nullptr;
	Handle = FGAEffectHandle::GenerateHandle(this);

	const_cast<FGAEffectContainer&>(InArraySerializer).ApplyFromPrediction(Handle, PredictionHandle);
}
void FGAEffect::PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer)
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
FGAEffect::FGAEffect(TSubclassOf<class UGAGameEffectSpec> GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffectClass(GameEffectIn),
	Context(ContextIn)
{
	OwnedTags = GetEffect()->OwnedTags;
	Extension = nullptr;
	if (GetEffect()->Extension)
	{
		Extension = NewObject<UGAEffectExtension>(Context.Target.Get(), GetEffect()->Extension);
		Extension->OwningComponent = Context.GetTargetEffectsComponent();
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
}

FGAEffect::~FGAEffect()
{
	if (Extension)
	{
		Extension->MarkPendingKill();
	}
}
void FGAEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}

void FGAEffect::OnApplied()
{
	AppliedTime = TargetWorld->TimeSeconds;
	if (Extension)
	{
		Extension->NativeOnEffectApplied();
	}
}
void FGAEffect::OnExecuted()
{
	if (Extension)
	{
		Extension->NativeOnEffectExecuted();
	}
}
void FGAEffect::OnExpired()
{
	if (Extension)
	{
		Extension->NativeOnEffectExpired();
	}
}
void FGAEffect::OnRemoved()
{
	if (Extension)
	{
		Extension->NativeOnEffectRemoved();
	}
}

float FGAEffect::GetDurationTime() const
{
	return GetFloatFromAttributeMagnitude(GetEffect()->Duration);
}
float FGAEffect::GetPeriodTime() const
{
	return GetFloatFromAttributeMagnitude(GetEffect()->Period);
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

float FGAEffect::GetCurrentDuration() const
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - AppliedTime;
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
	bool bHasDuration = InProperty.GetDuration() > 0;
	bool bHasPeriod = InProperty.GetPeriod() > 0;
	ENetRole role = OwningComponent->GetOwnerRole();
	ENetMode mode = OwningComponent->GetOwner()->GetNetMode();

	if (bHasDuration || bHasPeriod)
	{
		Handle = FGAEffectHandle::GenerateHandle(EffectIn);
	}
	if (InProperty.CanApply(EffectIn, this, InContext, Handle))
	{
		if(!bHasDuration && !bHasPeriod) //instatnt effect.
		{
			if (InProperty.IsHandleValid(InContext.Target.Get()))
			{
				Handle = InProperty.GetHandle(InContext.Target.Get());
				if (InProperty.ApplyEffect(Handle,
					EffectIn, this, InContext))
				{
					InProperty.ExecuteEffect(Handle, InContext, Modifier);
					//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
				}
			}
			else
			{
				Handle = FGAEffectHandle::GenerateHandle(EffectIn);
				EffectIn->Handle = Handle;
				if (InProperty.ApplyEffect(Handle,
					EffectIn, this, InContext))
				{
					InProperty.ExecuteEffect(Handle, InContext, Modifier);
					//	UE_LOG(GameAttributes, Log, TEXT("FGAEffectContainer::EffectApplied %s"), *HandleIn.GetEffectSpec()->GetName() );
				}
			}
		}
		else
		{
			EffectIn->Handle = Handle;
			if (InProperty.ApplyEffect(Handle,
				EffectIn, this, InContext))
			{
				InProperty.ExecuteEffect(Handle, InContext, Modifier);
				//generate it only on client, and apply prediction key from client.
				//if server replicates with valid key, then nothing happens.
				//if not we try to rewind effect application.
				//we probabaly don't need to unwind attribute changes, since next replication from
				//server will overridem them anyway.
				
				bool bIsServer = GEngine->GetNetMode(OwningComponent->GetWorld()) == ENetMode::NM_DedicatedServer;
				UE_LOG(AbilityFramework, Log, TEXT("%s :: FGAEffectContainer::EffectApplied %s"), bIsServer ? TEXT("Server") : TEXT("Client"),  *Handle.GetEffectSpec()->GetName() );
			}
		}
		
	}
	if (bHasDuration || bHasPeriod)
	{
		EffectIn->PredictionHandle = InProperty.GetPredictionHandle();
		EffectIn->AppliedTime = OwningComponent->GetWorld()->TimeSeconds;
		EffectIn->LastTickTime = OwningComponent->GetWorld()->TimeSeconds;
		MarkItemDirty(*EffectIn);
		ActiveEffectInfos.Add(*EffectIn);
		MarkArrayDirty();

		HandleByPrediction.Add(InProperty.GetPredictionHandle(), Handle);
		PredictionByHandle.Add(Handle, InProperty.GetPredictionHandle());
	}
	
	if (InProperty.GetSpec()->IfHaveTagEffect.RequiredTag.IsValid()
		&& InContext.GetTargetEffectsComponent()->HasTag(InProperty.GetSpec()->IfHaveTagEffect.RequiredTag))
	{
		for (TSubclassOf<class UGAGameEffectSpec>& Effect : InProperty.GetSpec()->IfHaveTagEffect.Effects)
		{
			FGAEffectProperty prop(Effect);
			UGABlueprintLibrary::ApplyEffect(prop, InContext.Target.Get(), InContext.Instigator.Get(), InContext.Causer.Get(), InContext.HitResult);
		}
	}

	return Handle;
	
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
	UGAGameEffectSpec* Spec = InProperty.GetSpec();
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

void FGAEffectContainer::AddEffect(FGAEffectProperty& InProperty, const FGAEffectHandle& HandleIn, bool bInfinite)
{
	TSet<FGAEffectHandle>& AttributeEffect = EffectByAttribute.FindOrAdd(HandleIn.GetAttribute());
	AttributeEffect.Add(HandleIn);
	ActiveEffectHandles.Add(HandleIn);
	AddEffectByClass(HandleIn);
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	FObjectKey key(InProperty.GetClass());
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
	UAFEffectsComponent* Target = HandleIn.GetContextRef().GetTargetEffectsComponent();
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
	
	const_cast<FGAEffectProperty&>(InProperty).RemoveHandle(HandleIn);

	
	
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
	FGAEffectHandle* Out = ActiveEffectHandles.Find(HandleIn);
	if (Out)
	{
		//FString EffectInfoLog(TEXT("FGAEffectContainer::RemoveEffect "));
		//EffectInfoLog += Out->GetEffectTag().ToString();
		//AddLogDebugInfo(EffectInfoLog, OwningComponent->GetWorld());
		FGAEffect* Effect = Out->GetEffectPtr();
		MarkItemDirty(*Effect);
		ActiveEffectInfos.Remove(Out->GetEffect());
		MarkArrayDirty();
	}
	ActiveEffectHandles.Remove(HandleIn);
	FAFPredictionHandle* PredHandle = PredictionByHandle.Find(HandleIn);
	if(PredHandle)
	{
		PredictedEffectInfos.Remove(*PredHandle);
		HandleByPrediction.Remove(*PredHandle);
		PredictionByHandle.Remove(HandleIn);
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
	FGAEffect* Effect = HandleIn.GetEffectPtr();
	if (HandlesToRemove)
	{
		HandlesToRemove->Remove(HandleIn);
		if (HandlesToRemove->Num() <= 0)
		{
			InstigatorEffect->Remove(EffectClass);
		}
	}
	if (Effect)
	{
		Target->RemoveTagContainer(Effect->ApplyTags);
		FTimerManager& DurationTimer = Effect->Context.TargetComp->GetWorld()->GetTimerManager();
		DurationTimer.ClearTimer(Effect->DurationTimerHandle);
		DurationTimer.ClearTimer(Effect->PeriodTimerHandle);
	}
	RemoveEffectProtected(HandleIn, InProperty);

}

void FGAEffectContainer::RemoveEffectByHandle(const FGAEffectHandle& InHandle, const FGAEffectProperty& InProperty)
{
	UGAGameEffectSpec* Spec = InProperty.GetSpec();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	//TSet<FGAEffectHandle>* handles = EffectByClass.Find(FObjectKey(HandleIn.GetClass()));//GetHandlesByClass(HandleIn, InContext);

	if (!ActiveEffectHandles.Contains(InHandle))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *InHandle.GetEffect().ToString());
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
	
	FGAEffect* Effect = HandleIn.GetEffectPtr();

	if (Handles)
	{
		Handles->Remove(HandleIn);
		if (Handles->Num() <= 0)
		{
			TargetEffectByClass.Remove(EffectClass);
		}
	}

	RemoveEffectProtected(HandleIn, InProperty);
	if (Effect)
	{
		Target->RemoveTagContainer(Effect->ApplyTags);
		FTimerManager& DurationTimer = Effect->Context.TargetComp->GetWorld()->GetTimerManager();
		DurationTimer.ClearTimer(Effect->DurationTimerHandle);
		DurationTimer.ClearTimer(Effect->PeriodTimerHandle);
	}
}

TArray<FGAEffectHandle> FGAEffectContainer::RemoveEffect(const FGAEffectProperty& HandleIn, int32 Num)
{
	UGAGameEffectSpec* Spec = HandleIn.GetSpec();
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	FObjectKey key(HandleIn.GetClass());
	TArray<FGAEffectHandle>* handles = EffectByClass.Find(key);//GetHandlesByClass(HandleIn, InContext);
	
	if (!handles)
		return TArray<FGAEffectHandle>();
	TArray<FGAEffectHandle> copy = *handles;

	for (int32 idx = 0; idx < Num; idx++)
	{
		if (handles->IsValidIndex(0))
		{
			FGAEffectHandle OutHandle = (*handles)[idx];
			if (OutHandle.IsValid())
			{
				if (!ActiveEffectHandles.Contains(OutHandle))
				{
					UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *OutHandle.GetEffect().ToString());
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
	return copy;
}

bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn)
{
	if (ActiveEffectHandles.Contains(HandleIn))
	{
		return true;
	}
	return false;
}
bool FGAEffectContainer::IsEffectActive(const FGAEffectHandle& HandleIn) const
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

void FGAEffectContainer::ApplyFromPrediction(const FGAEffectHandle& InHandle, const FAFPredictionHandle& InPredHandle)
{

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
	if (InHandle.IsValid())
	{
		float Duration = InHandle.GetEffectPtr()->GetDurationTime();
		return FMath::Clamp<float>(Duration - InHandle.GetEffectPtr()->GetCurrentDuration(), 0, Duration);
	}
	return 0;
}
float FGAEffectContainer::GetRemainingTimeNormalized(const FGAEffectHandle& InHandle) const
{
	if (InHandle.IsValid())
	{
		float CurrentDuration = InHandle.GetEffectPtr()->GetCurrentDuration();
		float MaxDuration = InHandle.GetEffectPtr()->GetDurationTime();

		float CurrentTime = ((CurrentDuration / MaxDuration) - 1) * (-1);

		return CurrentTime;// FMath::Clamp<float>(CurrentTime, 1, 0);
	}
	return 0;
}
float FGAEffectContainer::GetCurrentTime(const FGAEffectHandle& InHandle) const
{
	if (InHandle.IsValid())
		return InHandle.GetEffectPtr()->GetCurrentDuration();
	return 0;
}
float FGAEffectContainer::GetCurrentTimeNormalized(const FGAEffectHandle& InHandle) const
{
	if (InHandle.IsValid())
	{
		float CurrentDuration = InHandle.GetEffectPtr()->GetCurrentDuration();
		float MaxDuration = InHandle.GetEffectPtr()->GetDurationTime();
		return CurrentDuration * 1 / MaxDuration;
	}
	return 0;
}
float FGAEffectContainer::GetEndTime(const FGAEffectHandle& InHandle) const
{
	return 0;
}