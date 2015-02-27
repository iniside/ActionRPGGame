// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Effects/GAEffect.h"

#include "GAAttributeComponent.h"

DEFINE_STAT(STAT_ApplyEffect);
DEFINE_STAT(STAT_ModifyAttribute);
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

void UGAAttributeComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	ActiveEffects.Clean();
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
	AppliedTags.AddTagContainer(DefaultTags);
}

FGAEffectHandle UGAAttributeComponent::ApplyEffectToSelf(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{
	return ActiveEffects.ApplyEffect(SpecIn, Context);
}
FGAEffectHandle UGAAttributeComponent::ApplyEffectToTarget(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{
	SCOPE_CYCLE_COUNTER(STAT_ApplyEffect);
	//ActiveEffects.ApplyEffectToTarget(SpecIn, Context);
	if (Context.TargetComp.IsValid()
		&& Context.InstigatorComp.IsValid())
	{
		return Context.TargetComp->ApplyEffectToSelf(SpecIn, Context);
	}
	return FGAEffectHandle();
}

FGAEffectHandle UGAAttributeComponent::ApplySelfEffect(AActor* Target, APawn* Instigator,
	UObject* Causer, FGAEffectSpec SpecIn)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target);
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return FGAEffectHandle();

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FGAEffectContext context(Target->GetActorLocation(), Target, Causer,
		Instigator, targetComp, instiComp);

	SpecIn.Context = context;

	SpecIn.GetModifiers();
	return ActiveEffects.ApplyEffect(SpecIn, context);
}

FGAEffectInstant UGAAttributeComponent::MakeOutgoingInstantEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{
	FGAEffectInstant InstatnEffect;
	//ApplyEffectToTarget(SpecIn, Context);
	return InstatnEffect;
}
void UGAAttributeComponent::MakeOutgoingPeriodicEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context)
{

}

void UGAAttributeComponent::EffectExpired(const FGAEffectHandle& HandleIn)
{
	ActiveEffects.RemoveActiveEffect(HandleIn);
}

void UGAAttributeComponent::ExecuteModifiers(FGAAttributeData& ModifierIn, const FGameplayTagContainer& EffectTags, 
	const FGAEffectContext Context)
{
	ActiveEffects.ExecuteEffectModifier(ModifierIn, EffectTags, Context);
}

void UGAAttributeComponent::ModifyAttributesOnSelf(TArray<FGAAttributeData>& EvalData, const FGAEffectContext& Context, 
	const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn)
{
	//incoming
	ModifiedAttribute.Empty();
	for (FGAAttributeData& eval : EvalData)
	{
		FGAAttributeBase* attr = DefaultAttributes->GetAttribute(eval.Attribute);
		float newValue = 0;
		if (attr)
		{
			if (HandleIn.IsValid())
			{
				DefaultAttributes->SetAttributeAdditiveBonus(eval.Attribute, eval.Value);
			}
		}
		else
		{
			newValue = DefaultAttributes->AttributeOperation(eval.Attribute, eval.Value, eval.Mod);
			FGAEvalData evalData(eval.Attribute, eval.Mod, eval.AttributeTag, newValue);
			float finalValue = DefaultAttributes->UpdateAttributes(evalData, newValue);
			FGAModifiedAttribute ModdedAttrRep;
			ModdedAttrRep.Attribute = eval.Attribute;
			ModdedAttrRep.InstigatorLocation = Context.Instigator->GetActorLocation();
			ModdedAttrRep.TargetLocation = Context.TargetHitLocation;//AttributeIn.Target->GetActorLocation();
			ModdedAttrRep.ModifiedByValue = finalValue;
			ModdedAttrRep.ReplicationCounter += 1;
			ModdedAttrRep.Causer = Context.InstigatorComp;
			ModifiedAttribute.Add(ModdedAttrRep);
		}

	}

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		for (FGAModifiedAttribute& attr : ModifiedAttribute)
		{
			Context.InstigatorComp->OnAttributeModifed.Broadcast(attr);
		}
	}
}
void UGAAttributeComponent::ModifyAttributesOnTarget(TArray<FGAAttributeData>& EvalData, const FGAEffectContext& Context, 
	const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn)
{
	SCOPE_CYCLE_COUNTER(STAT_ModifyAttribute);
	//outgoing 
	//if (Context.TargetComp.IsValid()
	//	&& Context.InstigatorComp.IsValid())
	//{
	//for (FGAAttributeData& eval : EvalData)
	//{
	//	ExecuteModifiers(eval, EffectTags, Context);
	//	eval.ModDirection = EGAModifierDirection::Incoming;
	//}
	Context.TargetComp->ModifyAttributesOnSelf(EvalData, Context, EffectTags, HandleIn);
	//}
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
