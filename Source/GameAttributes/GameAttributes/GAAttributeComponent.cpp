// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GAEffectCue.h"
#include "GAAttributeComponent.h"
#include "GAGameEffect.h"

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
	AppliedTags.AddTagContainer(DefaultTags);
	FGAGameEffect Efffect;
	for(const FGAGameEffectModifier& mod : ModifierTest)
	{
		GameEffectContainer.ApplyModifier(mod, Efffect);
	}
	//"Damage.Fire"
	//"Damage.Ice"
}

FGAEffectHandle UGAAttributeComponent::ApplyEffectToSelf(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	GameEffectContainer.ApplyEffect(EffectIn, HandleIn);

	//ExecuteEffect(EffectIn);
	return FGAEffectHandle();
}
FGAEffectHandle UGAAttributeComponent::ApplyEffectToTarget(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	if(EffectIn.IsValid())
		return EffectIn.Context.TargetComp->ApplyEffectToSelf(EffectIn, HandleIn);
	return FGAEffectHandle();
}

FGAGameEffectHandle UGAAttributeComponent::MakeGameEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	const FGAEffectContext& ContextIn)
{
	FGAGameEffect* effect = new FGAGameEffect(SpecIn.GetDefaultObject(), ContextIn);
	FGAGameEffectHandle handle = FGAGameEffectHandle::GenerateHandle(effect);
	//effect->Handle = handle;
	return handle;
}

void UGAAttributeComponent::ExecuteEffect(FGAGameEffect& EffectIn, EGAModifierApplication ModAppType)
{
	/* 
		this patth will give effects chance to do any replicated events, like applying cues. 
		WE do not make any replication at the ApplyEffect because some effect might want to apply cues
		on periods on expiration etc, and all those will go trouch ExecuteEffect path.
	*/
	GameEffectContainer.ExecuteEffect(EffectIn, ModAppType);
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
void UGAAttributeComponent::MulticastRemoveEffectCue_Implementation(int32 Handle)
{
	ActiveCues.CueRemoved(FGAEffectHandle(Handle));
}

void UGAAttributeComponent::MulticastEffectCueExpired_Implementation(int32 Handle)
{
	ActiveCues.CueExpired(FGAEffectHandle(Handle));
}

void UGAAttributeComponent::RemoveEffectCue(int32 Handle)
{
	ActiveCues.CueRemoved(FGAEffectHandle(Handle));
}

void UGAAttributeComponent::EffectExpired(const FGAEffectHandle& HandleIn)
{

}
void UGAAttributeComponent::EffectRemoved(const FGAEffectHandle& HandleIn)
{

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
		for (FGAModifiedAttribute& attr : ModifiedAttribute.Mods)
		{
			Context.InstigatorComp->OnAttributeModifed.Broadcast(attr);
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
void UGAAttributeComponent::ApplyEffectCue(int32 Handle)
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
