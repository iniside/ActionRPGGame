// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Effects/GAEffect.h"

#include "GAAttributeComponent.h"

//will have single struct param, with data about how attribute changed.


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
void UGAAttributeComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (DefaultAttributes)
	{
		DefaultAttributes->InitializeAttributes();
	}
	
	
}

bool UGAAttributeComponent::ApplyEffectToSelf(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{
	ActiveEffects.ApplyEffect(SpecIn, Context);
	return false;
}
bool UGAAttributeComponent::ApplyEffectToTarget(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{
	Context.TargetComp->ApplyEffectToSelf(SpecIn, Context);
	return false;
}

void UGAAttributeComponent::EffectExpired(FGAEffectHandle& HandleIn)
{

}

void UGAAttributeComponent::ModifyAttributesOnSelf(TArray<FGAEvalData>& EvalData, const FGAEffectContext& Context, const FGAEffectHandle& HandleIn)
{
	ModifiedAttribute.Empty();
	for (FGAEvalData& eval : EvalData)
	{
		FGAAttributeBase* attr = DefaultAttributes->GetAttribute(eval.Attribute);
		float newValue = 0;
		if (attr)
		{
			attr->AddBonus(FGAModifier(eval.Mod, eval.Value), HandleIn);
		}
		//and for now forget about other cases.
		else
		{
			newValue = DefaultAttributes->AttributeOperation(eval.Attribute, eval.Value, eval.Mod);
		}
		DefaultAttributes->UpdateAttributes(eval, newValue);
		FGAModifiedAttribute ModdedAttrRep;
		ModdedAttrRep.Attribute = eval.Attribute;
		ModdedAttrRep.InstigatorLocation = Context.Instigator->GetActorLocation();
		ModdedAttrRep.TargetLocation = Context.TargetHitLocation;//AttributeIn.Target->GetActorLocation();
		ModdedAttrRep.ModifiedByValue = eval.Value;
		ModdedAttrRep.ReplicationCounter += 1;
		ModdedAttrRep.Causer = Context.InstigatorComp;
		ModifiedAttribute.Add(ModdedAttrRep);
	}

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		for (FGAModifiedAttribute& attr : ModifiedAttribute)
		{
			Context.InstigatorComp->OnAttributeModifed.Broadcast(attr);
		}
	}
	
}
void UGAAttributeComponent::ModifyAttributesOnTarget(TArray<FGAEvalData>& EvalData, const FGAEffectContext& Context, const FGAEffectHandle& HandleIn)
{
	Context.TargetComp->ModifyAttributesOnSelf(EvalData, Context, HandleIn);
}

void UGAAttributeComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UGAAttributeComponent, DefaultAttributes);
	DOREPLIFETIME(UGAAttributeComponent, ModifiedAttribute);
}

void UGAAttributeComponent::OnRep_AttributeChanged()
{
	for (FGAModifiedAttribute& attr : ModifiedAttribute)
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
