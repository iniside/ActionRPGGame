// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GAEffectExecution.h"

#include "GAEffectCue.h"
#include "GAAttributeComponent.h"
#include "GAGameEffect.h"
#include "GAEffectInstanced.h"

DEFINE_STAT(STAT_ApplyEffect);
DEFINE_STAT(STAT_ModifyAttribute);

UGAAttributeComponent::UGAAttributeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bAutoRegister = true;
}
void UGAAttributeComponent::GetAttributeStructTest(FGAAttribute Name)
{
	DefaultAttributes->GetAttribute(Name);
}

void UGAAttributeComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	OnAttributeModifed.Clear();
}

void UGAAttributeComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (DefaultAttributes)
	{
		DefaultAttributes->OwningAttributeComp = this;
		DefaultAttributes->InitializeAttributes();
	}
	GameEffectContainer.OwningComp = this;
	ActiveCues.OwningComponent = this;
	AppliedTags.AddTagContainer(DefaultTags);
	FGAGameEffect Efffect;
	for(const FGAGameEffectModifier& Spec : ModifierTest)
	{
		FGAGameEffectHandle Handle = FGAGameEffectHandle::GenerateHandle(nullptr);
		GameEffectContainer.ApplyEffectModifier(Spec, Handle);
	}
	//"Damage.Fire"
	//"Damage.Ice"
}

FGAEffectHandle UGAAttributeComponent::ApplyEffectToSelf(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	OnEffectApplyToSelf.Broadcast(HandleIn, EffectIn.GameEffect->OwnedTags);
	GameEffectContainer.ApplyEffect(EffectIn, HandleIn);
	FGAEffectCueParams CueParams;
	CueParams.HitResult = EffectIn.Context.HitResult;
	OnEffectApplied.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	return FGAEffectHandle();
}
FGAEffectHandle UGAAttributeComponent::ApplyEffectToTarget(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	FGAEffectCueParams CueParams;
	CueParams.HitResult = EffectIn.Context.HitResult;
	//execute cue from effect regardless if we have target object or not.
	MulticastApplyEffectCue(HandleIn, EffectIn.GameEffect->EffectCue, CueParams);
	if (EffectIn.IsValid() && EffectIn.Context.TargetComp.IsValid())
	{
		OnEffectApplyToTarget.Broadcast(HandleIn, EffectIn.GameEffect->OwnedTags);
		return EffectIn.Context.TargetComp->ApplyEffectToSelf(EffectIn, HandleIn);
	}
	return FGAEffectHandle();
}

FGAGameEffectHandle UGAAttributeComponent::MakeGameEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	const FGAEffectContext& ContextIn)
{
	FGAGameEffect* effect = new FGAGameEffect(SpecIn.GetDefaultObject(), ContextIn);
	if (effect)
	{
		effect->OwnedTags.AppendTags(effect->GameEffect->OwnedTags);
		effect->ApplyTags.AppendTags(effect->GameEffect->ApplyTags);
	}
	FGAGameEffectHandle handle = FGAGameEffectHandle::GenerateHandle(effect);
	effect->Handle = &handle;
	return handle;
}

void UGAAttributeComponent::ApplyEffectForDuration(const FGAGameEffectHandle& HandleIn)
{
	FGAGameEffect& Effect = HandleIn.GetEffectRef();
	TArray<FGAEffectMod> Mods = Effect.GetAttributeModifiers();

	for (FGAEffectMod& mod : Mods)
	{
		FGAAttributeBase* attr = DefaultAttributes->GetAttribute(mod.Attribute);
		if (attr)
		{
			FGAModifier Modifier(mod.AttributeMod, mod.Value);
			float val = attr->GetFinalValue();
			UE_LOG(GameAttributes, Log, TEXT("Value Before bonus of attribute = %f"), val);
			attr->AddBonus(Modifier, HandleIn, EGAEffectStacking::Override);
			val = attr->GetFinalValue();
			UE_LOG(GameAttributes, Log, TEXT("Value After bonus of attribute = %f"), val);
		}
	}
}

void UGAAttributeComponent::ExecuteEffect(FGAGameEffectHandle HandleIn)
{
	/* 
		this patth will give effects chance to do any replicated events, like applying cues. 
		WE do not make any replication at the ApplyEffect because some effect might want to apply cues
		on periods on expiration etc, and all those will go trouch ExecuteEffect path.
	*/
	OnEffectExecuted.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Effect %s executed"), *HandleIn.GetEffectSpec()->GetName());
	FGAGameEffect& Effect = HandleIn.GetEffectRef();
	TArray<FGAEffectMod> Mods = Effect.GetAttributeModifiers();
	FGAExecutionContext ExecContext(Effect.Context.TargetComp.Get(), Effect.Context.TargetComp->DefaultAttributes,
		Effect.Context.InstigatorComp.Get(), Effect.Context.InstigatorComp->DefaultAttributes);
	//execute period regardless if this periodic effect ? Or maybe change name OnEffectExecuted ?
	Effect.OnEffectPeriod.ExecuteIfBound();
	MulticastExecuteEffectCue(HandleIn);
	for (FGAEffectMod& mod : Mods)
	{
		HandleIn.ExecuteEffect(HandleIn, mod, ExecContext);
		
	}

	//GameEffectContainer.ExecuteEffect(HandleIn, HandleIn.GetEffectRef());
}
void UGAAttributeComponent::ExpireEffect(FGAGameEffectHandle HandleIn)
{
	//call effect internal delegate:
	HandleIn.GetEffectPtr()->OnExpired();
	InternalRemoveEffect(HandleIn);
	OnEffectExpired.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
}
void UGAAttributeComponent::RemoveEffect(FGAGameEffectHandle& HandleIn)
{
	InternalRemoveEffect(HandleIn);
	OnEffectRemoved.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
}
void UGAAttributeComponent::InternalRemoveEffect(FGAGameEffectHandle& HandleIn)
{
	FTimerManager& timer = GetWorld()->GetTimerManager();
	timer.ClearTimer(HandleIn.GetEffectRef().PeriodTimerHandle);
	timer.ClearTimer(HandleIn.GetEffectRef().DurationTimerHandle);
	UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Reset Timers and Remove Effect"));
	
	FGAGameEffect& Effect = HandleIn.GetEffectRef();
	TArray<FGAEffectMod> Mods = Effect.GetAttributeModifiers();
	MulticastRemoveEffectCue(HandleIn);
	//periodic effects do not apply duration based modifiers to attributes.
	//yet in anycase.
	if (Effect.GameEffect->EffectType == EGAEffectType::Duration)
	{
		for (FGAEffectMod& mod : Mods)
		{
			FGAAttributeBase* attr = DefaultAttributes->GetAttribute(mod.Attribute);
			if (attr)
			{
				UE_LOG(GameAttributes, Log, TEXT("Value Before bonus of attribute = %f"), attr->GetFinalValue());
				attr->RemoveBonus(HandleIn);
				UE_LOG(GameAttributes, Log, TEXT("Value After bonus of attribute = %f"), attr->GetFinalValue());
			}
		}
	}
	GameEffectContainer.RemoveEffect(HandleIn);
}

void UGAAttributeComponent::ApplyInstacnedEffectToSelf(class UGAEffectInstanced* EffectIn)
{
	GameEffectContainer.ApplyEffectInstance(EffectIn);
}
void UGAAttributeComponent::ApplyInstancedToTarget(class UGAEffectInstanced* EffectIn)
{
	//FGAEffectCueParams CueParams;
	//CueParams.HitResult = EffectIn->Context.HitResult;
	//execute cue from effect regardless if we have target object or not.
	//MulticastApplyEffectCue(HandleIn, EffectIn->Cue, CueParams);
	EffectIn->Context.TargetComp->ApplyInstacnedEffectToSelf(EffectIn);
}

void UGAAttributeComponent::RemoveInstancedFromSelf(class UGAEffectInstanced* EffectIn)
{
	GameEffectContainer.RemoveEffectInstance(EffectIn);
}
TArray<FGAEffectUIData> UGAAttributeComponent::GetEffectUIData()
{
	TArray<FGAEffectUIData> dataReturn;
	return dataReturn;
}

int32 UGAAttributeComponent::GetEffectUIIndex()
{
	return 1;
}
FGAEffectUIData UGAAttributeComponent::GetEffectUIDataByIndex(int32 IndexIn)
{
	FGAEffectUIData data;
	return data;
}

void UGAAttributeComponent::MulticastApplyEffectCue_Implementation(FGAGameEffectHandle EffectHandle, TSubclassOf<UGAEffectCue> EffectCue, FGAEffectCueParams CueParams)
{
	ActiveCues.AddCue(EffectHandle, EffectCue, CueParams);
}

void UGAAttributeComponent::MulticastExecuteEffectCue_Implementation(FGAGameEffectHandle EffectHandle)
{
	ActiveCues.ExecuteCue(EffectHandle);
}

void UGAAttributeComponent::MulticastRemoveEffectCue_Implementation(FGAGameEffectHandle EffectHandle)
{
	ActiveCues.RemoveCue(EffectHandle);
}

void UGAAttributeComponent::MulticastUpdateDurationCue_Implementation(FGAGameEffectHandle EffectHandle, float NewDurationIn)
{

}
void UGAAttributeComponent::MulticastUpdatePeriodCue_Implementation(FGAGameEffectHandle EffectHandle, float NewPeriodIn)
{

}
void UGAAttributeComponent::MulticastUpdateTimersCue_Implementation(FGAGameEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn)
{

}
void UGAAttributeComponent::MulticastExtendDurationCue_Implementation(FGAGameEffectHandle EffectHandle, float NewDurationIn)
{

}


float UGAAttributeComponent::ModifyAttribute(FGAEffectMod& ModIn)
{
	return DefaultAttributes->ModifyAttribute(ModIn);
}

void UGAAttributeComponent::ModifyAttributesOnSelf(const FGAAttributeData& EvalData, const FGAEffectContext& Context,
	const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn)
{
	//incoming
	ModifiedAttribute.Mods.Empty();
	FGAAttributeData FinalData = DefaultAttributes->PreModifyAttribute(EvalData);
	FGAAttributeBase* attr = DefaultAttributes->GetAttribute(FinalData.Attribute);
	float newValue = 0;
	if (attr)
	{
		if (HandleIn.IsValid())
		{
			DefaultAttributes->SetAttributeAdditiveBonus(FinalData.Attribute, FinalData.Value);
		}
	}
	else
	{
		//newValue = DefaultAttributes->AttributeOperation(FinalData.Attribute, FinalData.Value, FinalData.Mod);
		FGAEvalData evalData(FinalData.Attribute, FinalData.Mod, FinalData.AttributeTag, FinalData.Value);
		float finalValue = DefaultAttributes->UpdateAttributes(evalData, FinalData.Value);
		FGAModifiedAttribute ModdedAttrRep;
		ModdedAttrRep.Attribute = FinalData.Attribute;
		ModdedAttrRep.InstigatorLocation = Context.Instigator->GetActorLocation();
		ModdedAttrRep.TargetLocation = Context.TargetHitLocation;//AttributeIn.Target->GetActorLocation();
		ModdedAttrRep.ModifiedByValue = finalValue;
		ModdedAttrRep.ReplicationCounter = FMath::RandRange(0,255);
		ModdedAttrRep.Causer = Context.InstigatorComp;
		ModifiedAttribute.ForceUpdate++;
		ModifiedAttribute.Mods.Add(ModdedAttrRep);
	}
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		for (FGAModifiedAttribute& modAttribute : ModifiedAttribute.Mods)
		{
			Context.InstigatorComp->OnAttributeModifed.Broadcast(modAttribute);
		}
	}
}
void UGAAttributeComponent::ModifyAttributesOnTarget(const FGAAttributeData& EvalData, const FGAEffectContext& Context,
	const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn)
{
	SCOPE_CYCLE_COUNTER(STAT_ModifyAttribute);

	Context.TargetComp->ModifyAttributesOnSelf(EvalData, Context, EffectTags, HandleIn);
}

void UGAAttributeComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UGAAttributeComponent, DefaultAttributes);
	DOREPLIFETIME(UGAAttributeComponent, ModifiedAttribute);
	
	DOREPLIFETIME(UGAAttributeComponent, ActiveCues);
}
void UGAAttributeComponent::OnRep_ActiveEffects()
{

}
void UGAAttributeComponent::OnRep_ActiveCues()
{

}

void UGAAttributeComponent::OnRep_AttributeChanged()
{
	for (FGAModifiedAttribute& attr : ModifiedAttribute.Mods)
	{
		attr.Causer->OnAttributeModifed.Broadcast(attr);
	}
}
bool UGAAttributeComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (DefaultAttributes)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(DefaultAttributes), *Bunch, *RepFlags);
	}
	return WroteSomething;
}
void UGAAttributeComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (DefaultAttributes && DefaultAttributes->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UGAAttributesBase*>(DefaultAttributes));
	}
}
