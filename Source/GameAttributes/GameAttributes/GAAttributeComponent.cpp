// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Effects/GAEffect.h"

#include "GAEffectCue.h"
#include "GAAttributeComponent.h"

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

FGAEffectHandle UGAAttributeComponent::ApplyEffectToSelf(TSubclassOf<class UGAEffectSpecification> SpecIn,
	const FGAEffectContext& Context, const FName& EffeftName)
{
	FGAEffectHandle handle;
	if (GetOwnerRole() < ROLE_Authority)
	{

	}
	else
	{
		handle = ActiveEffects.ApplyEffect(SpecIn, Context, EffeftName);
	}

	if (GetNetMode() == ENetMode::NM_Standalone)
		OnRep_ActiveEffects();

	return handle;
}

FGAEffectHandle UGAAttributeComponent::ApplyEffectToTarget(TSubclassOf<class UGAEffectSpecification> SpecIn,
	const FGAEffectContext& Context, const FName& EffeftName)
{
	return Context.TargetComp->ApplyEffectToSelf(SpecIn, Context, EffeftName);
}

FGAEffectHandle UGAAttributeComponent::ApplySelfEffect(AActor* Target, APawn* Instigator,
	UObject* Causer, FGAEffectSpec SpecIn)
{
	//this is bad btw. I need to change it. LAter.
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target);
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
		return FGAEffectHandle();

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FGAEffectContext context(Target->GetActorLocation(), Target, Causer,
		Instigator, targetComp, instiComp);

	SpecIn.Context = context;

	//SpecIn.GetModifiers();
	return FGAEffectHandle(); // ActiveEffects.ApplyEffect(SpecIn, context);
}

TArray<FGAEffectUIData> UGAAttributeComponent::GetEffectUIData()
{
	TArray<FGAEffectUIData> dataReturn;
	for (auto It = ActiveEffects.RepActiveEffects.CreateIterator(); It; ++It)
	{
		FGAEffectUIData data;
		data.RemainingTime = It->GetRemainingDuration(GetWorld()->GetTimeSeconds());
		dataReturn.Add(data);
	}
	return dataReturn;
}

int32 UGAAttributeComponent::GetEffectUIIndex()
{
	return ActiveEffects.RepActiveEffects.Num() - 1;
}
FGAEffectUIData UGAAttributeComponent::GetEffectUIDataByIndex(int32 IndexIn)
{
	FGAEffectUIData data;
	if (ActiveEffects.RepActiveEffects.IsValidIndex(IndexIn))
	{
		data.RemainingTime = ActiveEffects.RepActiveEffects[IndexIn].GetRemainingDuration(GetWorld()->GetTimeSeconds());
	}
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
	MulticastEffectCueExpired(HandleIn.GetHandle());
	ActiveEffects.RemoveActiveEffect(HandleIn);
}
void UGAAttributeComponent::EffectRemoved(const FGAEffectHandle& HandleIn)
{
	MulticastRemoveEffectCue(HandleIn.GetHandle());
	ActiveEffects.RemoveActiveEffect(HandleIn);
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
	
	DOREPLIFETIME(UGAAttributeComponent, ActiveEffects);
	DOREPLIFETIME(UGAAttributeComponent, ActiveCues);
}
void UGAAttributeComponent::OnRep_ActiveEffects()
{
	//because Last return copy, and I don't need copy.
	int32 LastIndex = ActiveEffects.RepActiveEffects.Num() - 1;
	//activate newly added effect.
	if (ActiveEffects.RepActiveEffects.IsValidIndex(LastIndex))
	{
		if (ActiveEffects.RepActiveEffects[LastIndex].bIsActivated
			|| !ActiveEffects.RepActiveEffects[LastIndex].CueClass)
			return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoCollisionFail = true;
		AGAEffectCue* cue = GetWorld()->SpawnActor<AGAEffectCue>(ActiveEffects.RepActiveEffects[LastIndex].CueClass,
			ActiveEffects.RepActiveEffects[LastIndex].Context.TargetHitLocation, FRotator(0, 0, 0), SpawnParams);
		
		FGAActiveEffectCue ActiveCue(ActiveEffects.RepActiveEffects[LastIndex].MyHandle, cue);
		ActiveCue.Context = ActiveEffects.RepActiveEffects[LastIndex].Context;
		ActiveCue.EffectCue = cue;
		ActiveCue.Duration = ActiveEffects.RepActiveEffects[LastIndex].Duration;
		ActiveCue.Period = ActiveEffects.RepActiveEffects[LastIndex].Period;
		ActiveCues.AddCue(ActiveCue);
	}
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
