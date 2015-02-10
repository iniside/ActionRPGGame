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

void UGAAttributeComponent::ApplyEffectToSelf(const FGAEffectContext& SpecIn, TSubclassOf<class UGAEffect> EffectIn)
{
	EffectIn.GetDefaultObject()->OnEffectApplied(SpecIn);
}
void UGAAttributeComponent::ApplyEffectToTarget(const FGAEffectContext& SpecIn, TSubclassOf<class UGAEffect> EffectIn)
{
	SpecIn.TargetComp->ApplyEffectToSelf(SpecIn, EffectIn);
}

bool UGAAttributeComponent::ApplyEffectToSelf(FGAEffectBase& SpecIn)
{
	/*
		1. If effects, which are present on target, have certain condtional executions,
		like if target attack, I will apply 20dmg to Target.
		Or if this Target is attacked by mele damage, I will deal 10dmg to target.
		We should allow it to execute those condtional effects right here.
		Effect can have any number of conditions, each condition is checked by tags of incoming effect.
		
		Since there is no good way (that I'm aware of), for sorting all effects,
		by tags, to which they can respond, probabaly best and only way to do it
		is to simply iterate over all active effects, and give each one of them chance to execute any
		additonal effects.


		2. Handle stacking changes to single attribute.
		Handling simple changes (like +40 health) is easy enough. We don't need to track anything.
		When effect is applied we simply add +40 hp to Health and MaxHealth, and when
		effect is removed we just subtract it.
		The tricky ones are precentage.
		There are at least several ways to calculate them:
		a) From current value (which include all other modifiers).
		b) From base value (it will only use base default health no modifiers included).
		c) From base value + static mods (static mods, are those which lasts infinite amount of time,
		until player die or remove them manually).

		So if we have precentage increase by 10%, and then by 10% from 100hp we get:
		100 + 100 * 0.1 = 110;
		110 + 100 * 0.1 = 121
		and so on.
		
		The same methothology apply for reduction.
		We can also calculate only from base:
		100 + 100 * 0.1 = 110;
		110 + 100 * 0.1 = 120;

		What's worse Assume we have two effects. One +50hp and other +10% hp;
		So while first effect is active we have
		100 + 50 = 150hp
		150 + 150 * 0.1 = 165hp;
		Now if the +50hp mod is removed we want to recalculate second mod to look like this:
		100 + 100 * 0.1 = 110hp

		Regardless of how modifier modify attribute (static change or percentage), we need to store
		modifier value on itself, so when it is removed we can add or subtract this value
		from modifier attribute.

		So every time attribute with modifiers, change we need:
		1. Callback all effects, which modify this attribute to recalculate their mods. 
		For start we want to recalculate all mods linearlly (in order that mods are in Array).
		With callback don't need to send anything in particular. Modifier allready know, which
		effect they modify, all they need to do is to recalculate their mods.

		2. When do want to Callback ? Not all cases qualify for it.
		It health changed because of damage taken, we don't want to callback and recalculate
		any bonuses, as they entire point of having bonuses is to have more health
		to absorb more damage.
	*/

	if (SpecIn.IsOfType(FGAEffectInstant::EffectID))
	{
		FGAEffectInstant* spec = (FGAEffectInstant*)&SpecIn;
		spec->CalculateMagnitude();
		ActiveEffects.ApplyEffect(spec->EffectContext, spec->AttributeData);
		
		spec->ApplyInstantEffect();
		DefaultAttributes->PostEffectApplied(SpecIn);
		return true;
	}
	else if (SpecIn.IsOfType(FGAEffectDuration::EffectID))
	{
		FGAEffectDuration* spec = (FGAEffectDuration*)&SpecIn;
		DefaultAttributes->PostEffectApplied(SpecIn);
		TSharedPtr<FGAEffectDuration> tempEff = MakeShareable(new FGAEffectDuration(*spec));
		FGAEffectHandle newHandle = FGAEffectHandle::GenerateHandle();
		tempEff->Handle = newHandle;
		ActiveEffects.Effects.Add(newHandle, tempEff);

		tempEff->ApplyEffect();
		//do not add transitent attributes, like Damage
		//here since they do not have any persistent meaning.
		//ActiveEffects.ApplyEffect(spec->EffectContext, spec->AttributeSpec.OnAppliedSpec);
		//AppliedTags.AddTag(spec->MyTag);

		//spec->RegisterDelegates(OnPostEffectApplied, OnPostEffectRemoved);
	}
	return false;
}
bool UGAAttributeComponent::ApplyEffectToTarget(FGAEffectBase& SpecIn, UGAAttributeComponent* TargetIn)
{
	TargetIn->ApplyEffectToSelf(SpecIn);
	return false;
}

void UGAAttributeComponent::EffectExpired(FGAEffectHandle& HandleIn)
{
	TSharedPtr<FGAEffectDuration> eff = ActiveEffects.Effects.FindAndRemoveChecked(HandleIn);
	
	//OnPostEffectRemoved.Broadcast(SpecIn);
	//DefaultAttributes->PostEffectRemoved(SpecIn);
}

FGAAttributeDataCallback UGAAttributeComponent::ModifyAttributesOnSelf(const FGAEffectContext& Context, const FGAEffectHandle& HandleIn, FGAAttributeModData& AttributeIn)
{

	//apply final value to attribute.
	//after we appiled
	ENetRole role = GetOwnerRole();
	ModifiedAttribute.Empty(); //don't need anything old laying around.
	TArray<FGAAttributeSpec> ModdedSpecs;
	FGAAttributeDataCallback Callback;
	for (FGAAttributeSpec& spec : AttributeIn.AttributeModSpec)
	{
		FGAAttributeSpec ModdedSpecOut = DefaultAttributes->CalculateIncomingAttributeMods(spec);
		ModdedSpecs.Add(ModdedSpecOut);
		//first we need to check with what kind of attribute we are dealing with.
		float CalculatedMag = ModdedSpecOut.GetCalculatedMagnitude();
		float newValue = 0;
		FGAAttributeBase* attr = DefaultAttributes->GetAttribute(spec.Attribute);
		//we are dealing with complex attribute.
		if (attr)
		{
			attr->AddBonus(spec.FinalMod, HandleIn);
		}
		//and for now forget about other cases.
		else
		{
			newValue = DefaultAttributes->AttributeOperation(ModdedSpecOut.Attribute, CalculatedMag, ModdedSpecOut.Operation);
		}
		
		
		
		FGAEvalData attrSet = FGAEvalData(Context);
		attrSet.Attribute = ModdedSpecOut.Attribute;
		attrSet.ModValue = ModdedSpecOut.GetCalculatedMagnitude();
		Callback = DefaultAttributes->UpdateAttributes(attrSet, newValue);
		
		FGAModifiedAttribute ModdedAttrRep;
		ModdedAttrRep.Attribute = ModdedSpecOut.Attribute;
		ModdedAttrRep.InstigatorLocation = Context.Instigator->GetActorLocation();
		ModdedAttrRep.TargetLocation = Context.Target.Location;//AttributeIn.Target->GetActorLocation();
		ModdedAttrRep.ModifiedByValue = ModdedSpecOut.GetCalculatedMagnitude();
		//ModdedAttrRep.Tags = AttributeIn.Tags;
		ModdedAttrRep.ReplicationCounter += 1;
		ModdedAttrRep.Causer = Context.InstigatorComp;
		ModifiedAttribute.Add(ModdedAttrRep);

		FGAUpdatedAttribute attributeUpdate;
		attributeUpdate.Attribute = ModdedSpecOut.Attribute;
		attributeUpdate.NewValue = newValue;
		OnAttributeUpdated.Broadcast(attributeUpdate);
	}
	AttributeIn.AttributeModSpec = ModdedSpecs;

	////check if if this actor is dead.
	//if (DefaultAttributes->GetFloatValue(DeathAttribute) <= 0)
	//{
	//	if (AttributeInterface)
	//	{
	//		AttributeInterface->Died();
	//	}
	//}
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		for (FGAModifiedAttribute& attr : ModifiedAttribute)
		{
			Context.InstigatorComp->OnAttributeModifed.Broadcast(attr);
		}
	}

	return Callback;
}
FGAAttributeDataCallback UGAAttributeComponent::ModifyAttributesOnTarget(const FGAEffectContext& Context, const FGAEffectHandle& HandleIn, FGAAttributeModData& AttributeIn)
{
	if (!DefaultAttributes)
		return FGAAttributeDataCallback();
	TArray<FGAAttributeSpec> ModdedSpecs;
	for (FGAAttributeSpec& spec : AttributeIn.AttributeModSpec)
	{
		FGAAttributeSpec ModdedSpecOut = DefaultAttributes->CalculateOutgoingAttributeMods(spec);

		ModdedSpecs.Add(ModdedSpecOut);
	}
	AttributeIn.AttributeModSpec = ModdedSpecs;

	return Context.TargetComp->ModifyAttributesOnSelf(Context, HandleIn, AttributeIn);
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
