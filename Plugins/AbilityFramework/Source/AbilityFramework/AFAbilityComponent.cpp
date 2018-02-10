// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"

#include "Abilities/GAAbilityBase.h"
#include "IAbilityFramework.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GAGlobals.h"
#include "Abilities/GAAbilitySet.h"
#include "Attributes/GAAttributesBase.h"
#include "AFAbilityInterface.h"
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Effects/GAEffectExtension.h"
#include "Effects/GAEffectCue.h"
#include "AFCueManager.h"
#include "Effects/GABlueprintLibrary.h"
#include "Async.h"
#include "AFAbilityComponent.h"



DEFINE_STAT(STAT_ApplyEffect);
DEFINE_STAT(STAT_ModifyAttribute);



void FAFReplicatedAttributeItem::PreReplicatedRemove(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{
	FAFReplicatedAttributeContainer& ArraySerializer = const_cast<FAFReplicatedAttributeContainer&>(InArraySerializer);
	ArraySerializer.AttributeMap.Remove(AttributeTag);
}
void FAFReplicatedAttributeItem::PostReplicatedAdd(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{
	FAFReplicatedAttributeContainer& ArraySerializer = const_cast<FAFReplicatedAttributeContainer&>(InArraySerializer);
	UGAAttributesBase*& Attribute = ArraySerializer.AttributeMap.FindOrAdd(AttributeTag);
	Attribute = Attributes;
	InArraySerializer.OnAttributeReplicated(AttributeTag);
}
void FAFReplicatedAttributeItem::PostReplicatedChange(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{

}
UGAAttributesBase* FAFReplicatedAttributeContainer::Add(const FGameplayTag InTag, UGAAttributesBase* InAttributes, class UAFAbilityComponent* InOuter)
{
	UGAAttributesBase* AttributesDup = DuplicateObject<UGAAttributesBase>(InAttributes, InOuter);
	FAFReplicatedAttributeItem Item;
	Item.AttributeTag = InTag;
	Item.Attributes = AttributesDup;
	Attributes.Add(Item);
	MarkItemDirty(Item);
	UGAAttributesBase*& Added = AttributeMap.FindOrAdd(InTag);
	Added = AttributesDup;
	return AttributesDup;
}
UAFAbilityComponent::UAFAbilityComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bIsAnyAbilityActive = false;
	bAutoActivate = true;
	bAutoRegister = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bRunOnAnyThread = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_DuringPhysics;
}

void UAFAbilityComponent::BroadcastAttributeChange(const FGAAttribute& InAttribute,
	const FAFAttributeChangedData& InData)
{
	FAFAttributeChangedDelegate* Delegate = AttributeChanged.Find(InAttribute);
	if (Delegate)
	{
		Delegate->Broadcast(InData);
	}
}

bool UAFAbilityComponent::GetShouldTick() const
{
	return true;
}

void UAFAbilityComponent::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	,FGAEffectProperty& InProperty)
{ 
	//OnAttributePreModifed.Broadcast(ModIn, 0);
	//Add log.
	float NewValue = DefaultAttributes->ModifyAttribute(ModIn, HandleIn, InProperty);
	FAFAttributeChangedData Data;
	FGAEffectContext& Context = HandleIn.GetContextRef();
	Data.Mod = ModIn;
	Data.Target = Context.Target;
	Data.Location = Context.HitResult.Location;
	OnAttributeModifed.Broadcast(Data);
	NotifyInstigatorTargetAttributeChanged(Data, Context);
};
void UAFAbilityComponent::NotifyInstigatorTargetAttributeChanged(const FAFAttributeChangedData& InData,
	const FGAEffectContext& InContext)
{
	InContext.InstigatorComp->OnTargetAttributeModifed.Broadcast(InData);
}
void UAFAbilityComponent::GetAttributeStructTest(FGAAttribute Name)
{
	DefaultAttributes->GetAttribute(Name);
}

void UAFAbilityComponent::OnRep_GameEffectContainer()
{
	float test = 0;
}
void UAFAbilityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DefaultAttributes)
	{
		DefaultAttributes->Tick(DeltaTime);
	}
}
void UAFAbilityComponent::Update()
{

}
void UAFAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	FAFEffectTimerManager::Get();
}
void UAFAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}
void UAFAbilityComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	
}

FGAEffectHandle UAFAbilityComponent::ApplyEffectToSelf(FGAEffect* EffectIn
	,FGAEffectProperty& InProperty, FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
	const FGameplayTagContainer& AppliedEventTags = InProperty.GetSpec()->AppliedEventTags;
	FAFEventData Data;
	
	for (const FGameplayTag& Tag : AppliedEventTags)
	{
		if (FAFEventDelegate* Event = EffectEvents.Find(Tag))
		{
			Event->Broadcast(Data);
		}
	}
	if (FAFEffectEvent* Delegate = OnEffectApplyToTarget.Find(InProperty.GetSpec()->EffectTag))
	{
		Delegate->ExecuteIfBound();
	}
	
	FGAEffectHandle Handle = GameEffectContainer.ApplyEffect(EffectIn, InProperty, InContext, Modifier);
	GameEffectContainer.MarkArrayDirty();
	OnEffectAppliedToSelf.Broadcast(Handle);
	return Handle;
}


FGAEffectHandle UAFAbilityComponent::ApplyEffectToTarget(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty, FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
	if(FAFEffectEvent* Delegate = OnEffectApplyToSelf.Find(InProperty.GetSpec()->EffectTag))
	{
		Delegate->ExecuteIfBound();
	}
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	
	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams;
		CueParams.Instigator = InContext.Instigator;
		CueParams.Causer = InContext.Causer;
		CueParams.HitResult = InContext.HitResult;
		CueParams.CueTags = InProperty.GetSpec()->Cues.CueTags;
		CueParams.Period = InProperty.Period;
		CueParams.Duration = InProperty.Duration;
		//if (mode == ENetMode::NM_Standalone
		//	|| role >= ENetRole::ROLE_Authority)
		{
			//AsyncTask(ENamedThreads::GameThread, [=]()
			//{
				MulticastApplyEffectCue(CueParams);
			//});
		}
	}
	FString prefix = "";
	if (mode == ENetMode::NM_Client)
	{
		prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
	}
	else if (mode == ENetMode::NM_DedicatedServer)
	{
		prefix = FString::Printf(TEXT("DedicatedServer: "));
	}
	UE_LOG(AbilityFramework, Log, TEXT("%s : ApplyEffectToTarget Owner: %s, Instigator: %s"),
		*prefix,
		*GetOwner()->GetName(),
		*InContext.Instigator->GetName()
	);
	//here, we should start attribute change prediction
	//either change attribute or apply effect which will do so

	//execute cue from effect regardless if we have target object or not.
	if (InContext.TargetComp.IsValid())
	{
		FGAEffectHandle Handle;
		Handle = InContext.TargetComp->ApplyEffectToSelf(EffectIn, InProperty, InContext, Modifier);
		if (!PropertyByHandle.Contains(Handle))
			PropertyByHandle.Add(Handle, &InProperty);
			
		OnEffectAppliedToTarget.Broadcast(Handle);
		if(InProperty.Duration == 0
			&& InProperty.Period == 0)
		{
			OnEffectRemoved.Broadcast(Handle);
		}
		return Handle;
	}
	return FGAEffectHandle();
}

void UAFAbilityComponent::OnAttributeModified(const FGAEffectMod& InMod, 
	const FGAEffectHandle& InHandle, UGAAttributesBase* InAttributeSet)
{
	
}
void UAFAbilityComponent::ExecuteEffect(FGAEffectHandle HandleIn, FGAEffectProperty InProperty
	,FAFFunctionModifier Modifier, FGAEffectContext InContex)
{
	/*
	this patth will give effects chance to do any replicated events, like applying cues.
	WE do not make any replication at the ApplyEffect because some effect might want to apply cues
	on periods on expiration etc, and all those will go trouch ExecuteEffect path.
	*/
	const FGameplayTagContainer& RequiredTags = InProperty.GetSpec()->RequiredTags;
	if (RequiredTags.Num() > 0)
	{
		if (!HasAll(RequiredTags))
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Effect %s not executed, require tags: %s"), *HandleIn.GetEffectSpec()->GetName(), *RequiredTags.ToString());
			return;
		}
	}

	//apply execution events:
	const FGameplayTagContainer& ExecuteEventTags = InProperty.GetSpec()->ExecuteEventTags;
	FAFEventData Data;
	for (const FGameplayTag& Tag : ExecuteEventTags)
	{
		if (FAFEventDelegate* Event = EffectEvents.Find(Tag))
		{
			Event->Broadcast(Data);
		}
	}

	//OnEffectExecuted.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Effect %s executed"), *HandleIn.GetEffectSpec()->GetName());
	FGAEffect& Effect = HandleIn.GetEffectRef();
	FGAEffectMod Mod = FAFStatics::GetAttributeModifier(InProperty.GetAttributeModifier()
		, InProperty.GetSpec()
		, InContex
		, HandleIn);

	//execute period regardless if this periodic effect ? Or maybe change name OnEffectExecuted ?
	
	Effect.OnExecuted();
	MulticastExecuteEffectCue(HandleIn);
	InProperty.Execution->ExecuteEffect(HandleIn, Mod, HandleIn.GetContextRef(), InProperty, Modifier);
	PostExecuteEffect();
}
void UAFAbilityComponent::PostExecuteEffect()
{

}
void UAFAbilityComponent::ExpireEffect(FGAEffectHandle HandleIn, FGAEffectProperty InProperty,
	FGAEffectContext InContext)
{
	//call effect internal delegate:
	HandleIn.GetEffectPtr()->OnExpired();
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	
	if (mode == ENetMode::NM_DedicatedServer
		|| mode == ENetMode::NM_ListenServer)
	{
		ClientExpireEffect(InProperty.PredictionHandle);
	}

	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams;
		CueParams.Instigator = InContext.Instigator;
		CueParams.Causer = InContext.Causer;
		CueParams.HitResult = InContext.HitResult;
		CueParams.CueTags = InProperty.GetSpec()->Cues.CueTags;
		CueParams.Period = InProperty.Period;
		CueParams.Duration = InProperty.Duration;
		
		//if (mode == ENetMode::NM_Standalone
		//	|| role >= ENetRole::ROLE_Authority)
		{
			MulticastRemoveEffectCue(CueParams);
		}
	}
//	if (role < ENetRole::ROLE_Authority
//		|| mode == ENetMode::NM_Standalone)
	{
		FAFEffectRepInfo* RepInfo = GameEffectContainer.GetReplicationInfo(HandleIn);
		if (RepInfo)
		{
			RepInfo->OnExpired();
		}
	}
	TArray<FGAEffectHandle> handles = GameEffectContainer.RemoveEffect(InProperty);
	for (const FGAEffectHandle& Handle : handles)
	{
		OnEffectExpired.Broadcast(Handle);
	}
}

void UAFAbilityComponent::ClientExpireEffect_Implementation(FAFPredictionHandle PredictionHandle)
{

}

void UAFAbilityComponent::RemoveEffect(const FGAEffectProperty& InProperty, 
	const FGAEffectContext& InContext, const FGAEffectHandle& InHandle)
{
	{
		FAFEffectRepInfo* RepInfo = GameEffectContainer.GetReplicationInfo(InHandle);
		if (RepInfo && 
			(RepInfo->Type == ERepInfoType::LocallyPredicted
				|| RepInfo->Type == ERepInfoType::Server))
		{
			RepInfo->OnExpired();
		}
	}
	if (GetOwnerRole() == ENetRole::ROLE_Authority
		|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		InternalRemoveEffect(InProperty, InContext);
	}
	FGAEffectCueParams CueParams;
	CueParams.Instigator = InContext.Instigator;
	CueParams.Causer = InContext.Causer;
	CueParams.HitResult = InContext.HitResult;
	CueParams.CueTags = InProperty.GetSpec()->Cues.CueTags;
	CueParams.Period = InProperty.Period;
	CueParams.Duration = InProperty.Duration;
	MulticastRemoveEffectCue(CueParams);
}
void UAFAbilityComponent::InternalRemoveEffect(const FGAEffectProperty& InProperty, 
	const FGAEffectContext& InContext)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Reset Timers and Remove Effect"));

	//MulticastRemoveEffectCue(HandleIn);
	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams;
		CueParams.Instigator = InContext.Instigator;
		CueParams.Causer = InContext.Causer;
		CueParams.HitResult = InContext.HitResult;
		CueParams.CueTags = InProperty.GetSpec()->Cues.CueTags;
		CueParams.Period = InProperty.Period;
		CueParams.Duration = InProperty.Duration;
		ENetRole role = GetOwnerRole();
		ENetMode mode = GetOwner()->GetNetMode();
		if (mode == ENetMode::NM_Standalone
			|| role >= ENetRole::ROLE_Authority)
		{
			//MulticastRemoveEffectCue(CueParams);
		}
	}
	
	TArray<FGAEffectHandle> handles = GameEffectContainer.RemoveEffect(InProperty);
	for(const FGAEffectHandle& Handle : handles)
	{
		OnEffectRemoved.Broadcast(Handle);
	}
}


void UAFAbilityComponent::MulticastApplyEffectCue_Implementation( FGAEffectCueParams CueParams)
{
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	if(mode == ENetMode::NM_Standalone
		|| mode != ENetMode::NM_DedicatedServer)
	{
		FString prefix = "";
		if (mode == ENetMode::NM_Client)
		{
			int32 client = GPlayInEditorID - 1;
			if (client == 2)
			{
				float dupa = 0;
			}
			prefix = FString::Printf(TEXT("Client %d: "), client);
		}
		UE_LOG(AbilityFramework, Log, TEXT("%s : MulticastApplyEffectCue Owner: %s, Instigator: %s" ),
			*prefix, 
			*GetOwner()->GetName(),
			*CueParams.Instigator->GetName()
		);
		
		UAFCueManager::Get()->HandleCue(CueParams.CueTags, CueParams);
	}
}

void UAFAbilityComponent::MulticastExecuteEffectCue_Implementation(FGAEffectHandle EffectHandle)
{
	//	ActiveCues.ExecuteCue(EffectHandle);
}

void UAFAbilityComponent::MulticastRemoveEffectCue_Implementation(FGAEffectCueParams CueParams)
{
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	//if (mode == ENetMode::NM_Standalone
	//	|| role != ENetRole::ROLE_Authority)
	{
		UAFCueManager::Get()->HandleRemoveCue(CueParams.CueTags, CueParams);
	}
}

void UAFAbilityComponent::MulticastUpdateDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}
void UAFAbilityComponent::MulticastUpdatePeriodCue_Implementation(FGAEffectHandle EffectHandle, float NewPeriodIn)
{

}
void UAFAbilityComponent::MulticastUpdateTimersCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn)
{

}
void UAFAbilityComponent::MulticastExtendDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}

void UAFAbilityComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UAFAbilityComponent, DefaultAttributes);
	DOREPLIFETIME(UAFAbilityComponent, RepAttributes);
	DOREPLIFETIME(UAFAbilityComponent, GameEffectContainer);

	DOREPLIFETIME(UAFAbilityComponent, ActiveCues);
	DOREPLIFETIME(UAFAbilityComponent, AbilityContainer);
	//DOREPLIFETIME_CONDITION(UAFAbilityComponent, AbilityContainer, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAFAbilityComponent, RepMontage, COND_SkipOwner);
	//DOREPLIFETIME(UAFAbilityComponent, RepMontage);
}
void UAFAbilityComponent::OnRep_ActiveEffects()
{

}
void UAFAbilityComponent::OnRep_ActiveCues()
{

}

bool UAFAbilityComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (DefaultAttributes)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(DefaultAttributes), *Bunch, *RepFlags);
	}
	for (const FGASAbilityItem& Ability : AbilityContainer.AbilitiesItems)
	{
		//if (Set.InputOverride)
		//	WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASInputOverride*>(Set.InputOverride), *Bunch, *RepFlags);

		if (Ability.Ability)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAbilityBase*>(Ability.Ability), *Bunch, *RepFlags);
	}

	for (const FAFReplicatedAttributeItem& Attribute : RepAttributes.Attributes)
	{
		//if (Set.InputOverride)
		//	WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASInputOverride*>(Set.InputOverride), *Bunch, *RepFlags);

		if (Attribute.Attributes)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(Attribute.Attributes), *Bunch, *RepFlags);
	}

	return WroteSomething;
}
void UAFAbilityComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (DefaultAttributes && DefaultAttributes->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UGAAttributesBase*>(DefaultAttributes));
	}
}


FAFEventDelegate& UAFAbilityComponent::GetTagEvent(FGameplayTag TagIn)
{
	FAFEventDelegate& Delegate = EffectEvents.FindChecked(TagIn);
	return Delegate;
}
void UAFAbilityComponent::NativeTriggerTagEvent(FGameplayTag TagIn, const FAFEventData& InEventData)
{
	FAFEventDelegate* Delegate = EffectEvents.Find(TagIn);
	if (Delegate)
	{
		if (Delegate->IsBound())
		{
			Delegate->Broadcast(InEventData);
		}
	}
}
void UAFAbilityComponent::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = AppliedTags.AllTags;
}
bool UAFAbilityComponent::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return AppliedTags.HasTag(TagToCheck);
}

bool UAFAbilityComponent::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AppliedTags.HasAll(TagContainer);
}

bool UAFAbilityComponent::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AppliedTags.HasAny(TagContainer);
}

bool UAFAbilityComponent::DenyEffectApplication(const FGameplayTagContainer& InTags)
{
	bool bDenyApplication = false;
	if (InTags.Num() > 0)
	{
		bDenyApplication = HasAll(InTags);
	}
	return bDenyApplication;
}

bool UAFAbilityComponent::HaveEffectRquiredTags(const FGameplayTagContainer& InTags)
{
	bool bAllowApplication = true;
	if (InTags.Num() > 0)
	{
		bAllowApplication = HasAll(InTags);
	}
	return bAllowApplication;
}
const bool FGASAbilityItem::operator==(const FGameplayTag& AbilityTag) const
{
	return Ability->AbilityTag == AbilityTag;
}
void FGASAbilityItem::PreReplicatedRemove(const struct FGASAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		FGASAbilityContainer& InArraySerializerC = const_cast<FGASAbilityContainer&>(InArraySerializer);
		//remove attributes
		//UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = nullptr;
		FGameplayTag AbilityTag = Ability->AbilityTag;
		InArraySerializerC.RemoveAbilityFromAction(AbilityTag);
		InArraySerializerC.AbilitiesInputs.Remove(AbilityTag);
	}
}
void FGASAbilityItem::PostReplicatedAdd(const struct FGASAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		//should be safe, since we only modify the non replicated part of struct.
		FGASAbilityContainer& InArraySerializerC = const_cast<FGASAbilityContainer&>(InArraySerializer);
		Ability->AbilityComponent = InArraySerializer.AbilitiesComp.Get();
		if (InArraySerializer.AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(InArraySerializer.AbilitiesComp->GetOwner());
			Ability->POwner = POwner;
			Ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			Ability->OwnerCamera = nullptr;
		}
		Ability->InitAbility();
		Ability->Attributes = nullptr;
		UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = attr;
		InArraySerializerC.AbilitiesInputs.Add(Ability->AbilityTag, Ability); //.Add(Ability->AbilityTag, Ability);
		InArraySerializerC.AbilitiesComp->NotifyOnAbilityReady(Ability->AbilityTag);
	}
}
void FGASAbilityItem::PostReplicatedChange(const struct FGASAbilityContainer& InArraySerializer)
{

}
void FGASAbilityContainer::SetBlockedInput(const FGameplayTag& InInput, bool bBlock)
{
	if (BlockedInput.Contains(InInput))
	{
		BlockedInput[InInput] = bBlock;
	}
	else
	{
		BlockedInput.Add(InInput, bBlock);
	}
}
UGAAbilityBase* FGASAbilityContainer::AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn,
	AActor* InAvatar)
{
	if (AbilityIn && AbilitiesComp.IsValid())
	{
		
		UGAAbilityBase* ability = NewObject<UGAAbilityBase>(AbilitiesComp->GetOwner(), AbilityIn);
		ability->AbilityComponent = AbilitiesComp.Get();
		if (AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(AbilitiesComp->GetOwner());
			ability->POwner = POwner;
			ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			ability->OwnerCamera = nullptr;
			ability->AvatarActor = InAvatar;
		}
		ability->InitAbility();
		FGameplayTag Tag = ability->AbilityTag;

		AbilitiesInputs.Add(Tag, ability);
		FGASAbilityItem AbilityItem;
		MarkItemDirty(AbilityItem);
		AbilityItem.Ability = ability;
		AbilitiesItems.Add(AbilityItem);
		MarkArrayDirty();
		if (AbilitiesComp->GetNetMode() == ENetMode::NM_Standalone
			|| AbilitiesComp->GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			AbilitiesComp->NotifyOnAbilityReady(ability->AbilityTag);
		}
	/*	if (ActionName.IsValid())
		{
			UInputComponent* InputComponent = AbilitiesComp->GetOwner()->FindComponentByClass<UInputComponent>();
			AbilitiesComp->BindAbilityToAction(InputComponent, ActionName, Tag);
		}*/
		return ability;
	}
	return nullptr;
}
void FGASAbilityContainer::RemoveAbility(const FGameplayTag& AbilityIn)
{
	int32 Index = AbilitiesItems.IndexOfByKey(AbilityIn);
	
	if (Index == INDEX_NONE)
		return;
	MarkItemDirty(AbilitiesItems[Index]);
	RemoveAbilityFromAction(AbilityIn);
	AbilitiesItems.RemoveAt(Index);
	MarkArrayDirty();
}
FGameplayTag FGASAbilityContainer::IsAbilityBoundToAction(const FGameplayTag& InInputTag)
{
	FGameplayTag Ability;
	FGameplayTag* AbilityTag = ActionToAbility.Find(InInputTag);
	if (AbilityTag)
	{
		Ability = *AbilityTag;
	}
	
	return Ability;
}
void FGASAbilityContainer::RemoveAbilityFromAction(const FGameplayTag& InAbilityTag)
{
	/*TArray<FGameplayTag> OutActions;
	AbilityToAction.RemoveAndCopyValue(InAbilityTag, OutActions);

	for(const FGameplayTag& Action : OutActions)
	{
		ActionToAbility.Remove(Action);
	}*/

	//AbilitiesInputs.Remove(InAbilityTag);
}
void FGASAbilityContainer::SetAbilityToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& InputTag : InInputTag)
	{
		if (ActionToAbility.Contains(InputTag))
		{
			FGameplayTag AbilityTag = ActionToAbility.FindRef(InputTag);
			UE_LOG(AbilityFramework, Log, TEXT("FGASAbilityContainer: Input %s is already abount to Ability %s"),
				*InputTag.ToString(),
				*AbilityTag.ToString()
			);

			//return;
		}

		FGameplayTag& AbilityTag = ActionToAbility.FindOrAdd(InputTag);
		AbilityTag = InAbilityTag;

		TArray<FGameplayTag>& ActionTag = AbilityToAction.FindOrAdd(InputTag);
		ActionTag.Add(InputTag);
	}
	if (!AbilitiesComp.IsValid())
		return;
	if (AbilitiesComp->GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		AbilitiesComp->ClientNotifyAbilityInputReady(InAbilityTag);
	}
}

UGAAbilityBase* FGASAbilityContainer::GetAbility(FGameplayTag TagIn)
{
	UGAAbilityBase* retAbility = AbilitiesInputs.FindRef(TagIn);
	return retAbility;
}
void FGASAbilityContainer::HandleInputPressed(FGameplayTag ActionName, const FAFPredictionHandle& InPredictionHandle)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	FGameplayTag abilityTag = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(abilityTag);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		ability->OnNativeInputPressed(ActionName, InPredictionHandle);
	}
}
void FGASAbilityContainer::HandleInputReleased(FGameplayTag ActionName)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	FGameplayTag abilityTag = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(abilityTag);
	if (ability)
	{
		ability->OnNativeInputReleased(ActionName);
	}
}

void FGASAbilityContainer::TriggerAbylityByTag(FGameplayTag InTag)
{
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(InTag);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		FAFPredictionHandle PredHandle = FAFPredictionHandle::GenerateClientHandle(AbilitiesComp.Get());
		ability->OnNativeInputPressed(FGameplayTag(), PredHandle);
	}
}

void UAFAbilityComponent::InitializeComponent()
{
	Super::InitializeComponent();
	//PawnInterface = Cast<IIGIPawn>(GetOwner());
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	AbilityContainer.AbilitiesComp = this;
	//EffectTimerManager = MakeShareable(new FAFEffectTimerManager());
	
	//EffectTimerManager.InitThread();
	if (DefaultAttributes)
	{
		DefaultAttributes->InitializeAttributes(this);
		DefaultAttributes->InitializeAttributesFromTable();
	}
	GameEffectContainer.OwningComponent = this;
	ActiveCues.OwningComp = this;
	//ActiveCues.OwningComponent = this;
	AppliedTags.AddTagContainer(DefaultTags);
	InitializeInstancedAbilities();

	AActor* MyOwner = GetOwner();
	if (!MyOwner || !MyOwner->IsTemplate())
	{
		ULevel* ComponentLevel = (MyOwner ? MyOwner->GetLevel() : GetWorld()->PersistentLevel);
	}
}
void UAFAbilityComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	//EffectTimerManager.Deinitialize();
	//EffectTimerManager.Reset();
	//GameEffectContainer
}
void UAFAbilityComponent::BindInputs(class UInputComponent* InputComponent)
{
	for (const FGameplayTag& Tag : AbilityInputs)
	{
		BindAbilityToAction(InputComponent, Tag);
	}
}
void UAFAbilityComponent::SetBlockedInput(const FGameplayTag& InInput, bool bBlock)
{
	AbilityContainer.SetBlockedInput(InInput, bBlock);
}
void UAFAbilityComponent::BP_BindAbilityToAction(FGameplayTag ActionName)
{
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	check(InputComponent);

	BindAbilityToAction(InputComponent, ActionName);
}
void UAFAbilityComponent::BindAbilityToAction(UInputComponent* InputComponent, FGameplayTag ActionName)
{
	check(InputComponent);

	if (!InputComponent)
		return;

	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Pressed);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputPressed, ActionName);
		InputComponent->AddActionBinding(AB);
	}

	// Released event
	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Released);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputReleased, ActionName);
		InputComponent->AddActionBinding(AB);
	}
	SetBlockedInput(ActionName, false);
}
void UAFAbilityComponent::SetAbilityToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag
	, const FAFOnAbilityReady& InputDelegate)
{
	for (const FGameplayTag& Tag : InInputTag)
	{
		if (AbilityContainer.IsAbilityBoundToAction(Tag).IsValid())
		{
			RemoveAbilityFromAction(InAbilityTag, FGameplayTag());
		}
	}

	AbilityContainer.SetAbilityToAction(InAbilityTag, InInputTag);
	ENetRole role = GetOwnerRole();

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		&& role == ENetRole::ROLE_AutonomousProxy)
	{
		if (InputDelegate.IsBound())
		{
			AddOnAbilityInputReadyDelegate(InAbilityTag, InputDelegate);
		}
		ServerSetAbilityToAction(InAbilityTag, InInputTag);
	}
}

FGameplayTag UAFAbilityComponent::IsAbilityBoundToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& Tag : InInputTag)
	{
		return AbilityContainer.IsAbilityBoundToAction(Tag);
		break;
	}
	return FGameplayTag();
}
void UAFAbilityComponent::RemoveAbilityFromAction(const FGameplayTag& InAbilityTag, const FGameplayTag& InInputTag)
{
	AbilityContainer.RemoveAbilityFromAction(InAbilityTag);
}
void UAFAbilityComponent::ServerSetAbilityToAction_Implementation(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		SetAbilityToAction(InAbilityTag, InInputTag, FAFOnAbilityReady());
	}
}
bool UAFAbilityComponent::ServerSetAbilityToAction_Validate(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	return true;
}

void UAFAbilityComponent::ClientNotifyAbilityInputReady_Implementation(FGameplayTag AbilityTag)
{
	NotifyOnAbilityInputReady(AbilityTag);
}

void UAFAbilityComponent::BP_InputPressed(FGameplayTag ActionName)
{
	NativeInputPressed(ActionName);
}
void UAFAbilityComponent::NativeInputPressed(FGameplayTag ActionName)
{
	FAFPredictionHandle PredHandle;
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		PredHandle = FAFPredictionHandle::GenerateClientHandle(this);
		AbilityContainer.HandleInputPressed(ActionName, PredHandle);
	}
	ServerNativeInputPressed(ActionName, PredHandle);
		
}

void UAFAbilityComponent::ServerNativeInputPressed_Implementation(FGameplayTag ActionName, FAFPredictionHandle InPredictionHandle)
{
	AbilityContainer.HandleInputPressed(ActionName, InPredictionHandle);
	//NativeInputPressed(ActionName);
}
bool UAFAbilityComponent::ServerNativeInputPressed_Validate(FGameplayTag ActionName, FAFPredictionHandle InPredictionHandle)
{
	return true;
}

void UAFAbilityComponent::BP_InputReleased(FGameplayTag ActionName)
{
	NativeInputReleased(ActionName);
}

void UAFAbilityComponent::NativeInputReleased(FGameplayTag ActionName)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		AbilityContainer.HandleInputReleased(ActionName);
	}
	ServerNativeInputReleased(ActionName);
}

void UAFAbilityComponent::ServerNativeInputReleased_Implementation(FGameplayTag ActionName)
{
	AbilityContainer.HandleInputReleased(ActionName);
}
bool UAFAbilityComponent::ServerNativeInputReleased_Validate(FGameplayTag ActionName)
{
	return true;
}
void UAFAbilityComponent::BP_AddAbilityFromTag(FGameplayTag InAbilityTag,
	AActor* InAvatar, TArray<FGameplayTag> InInputTag)
{
	NativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
}
void UAFAbilityComponent::NativeAddAbilityFromTag(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	FGameplayTag AlreadyBound = IsAbilityBoundToAction(InAbilityTag, InInputTag);
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
		if (AlreadyBound.IsValid())
		{
			for (const FGameplayTag& Input : InInputTag)
			{
				AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
			}
		}
	}
	else
	{
		if (AlreadyBound.IsValid())
		{
			AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
		}
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			TArray<FAssetData> AssetData;
			FARFilter Filter;
			Filter.TagsAndValues.Add("AbilityTagSearch", InAbilityTag.ToString());
			AssetRegistryModule.Get().GetAssets(Filter, AssetData);
			FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("Ability"), AssetData[0].AssetName);
			FPrimaryAssetTypeInfo Info;
			if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
			{
				FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFAbilityComponent::OnFinishedLoad, InAbilityTag, PrimaryAssetId, InAvatar);

				Manager->LoadPrimaryAsset(PrimaryAssetId,
					TArray<FName>(),
					del);
			}
		}
	}
	
}

void UAFAbilityComponent::ServerNativeAddAbilityFromTag_Implementation(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	NativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
}

bool UAFAbilityComponent::ServerNativeAddAbilityFromTag_Validate(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	return true;
}
void UAFAbilityComponent::BP_RemoveAbility(FGameplayTag TagIn)
{

}
void UAFAbilityComponent::NativeRemoveAbility(const FGameplayTag& InAbilityTag)
{
	AbilityContainer.RemoveAbility(InAbilityTag);
}
void UAFAbilityComponent::ServerNativeRemoveAbility_Implementation(FGameplayTag InAbilityTag)
{
	NativeRemoveAbility(InAbilityTag);
}

bool UAFAbilityComponent::ServerNativeRemoveAbility_Validate(FGameplayTag InAbilityTag)
{
	return true;
}
void UAFAbilityComponent::OnFinishedLoad(FGameplayTag InAbilityTag,
	FPrimaryAssetId InPrimaryAssetId, AActor* InAvatar)
{
	if (AbilityContainer.AbilityExists(InAbilityTag))
	{
		return;
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* loaded = Manager->GetPrimaryAssetObject(InPrimaryAssetId);
		TSubclassOf<UGAAbilityBase> cls = Cast<UClass>(loaded);
		if (cls)
		{
			InstanceAbility(cls, InAvatar);
		}

		{
			Manager->UnloadPrimaryAsset(InPrimaryAssetId);
		}
	}
}


UGAAbilityBase* UAFAbilityComponent::BP_GetAbilityByTag(FGameplayTag TagIn)
{
	UGAAbilityBase* retVal = AbilityContainer.GetAbility(TagIn);
	return retVal;
}
UGAAbilityBase* UAFAbilityComponent::InstanceAbility(TSubclassOf<class UGAAbilityBase> AbilityClass,
	AActor* InAvatar)
{
	if (AbilityClass)
	{
		UGAAbilityBase* ability = AbilityContainer.AddAbility(AbilityClass, InAvatar);
		AbilityContainer.MarkArrayDirty();
		return ability;
	}
	return nullptr;
}

void UAFAbilityComponent::OnRep_InstancedAbilities()
{
}
void UAFAbilityComponent::NotifyOnAbilityAdded(const FGameplayTag& InAbilityTag)
{
	OnAbilityAdded.Broadcast(InAbilityTag);
	
}
void UAFAbilityComponent::InitializeInstancedAbilities()
{
}

void UAFAbilityComponent::OnRep_PlayMontage()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(RepMontage.CurrentMontage);
		if (RepMontage.SectionName != NAME_None)
		{
			AnimInst->Montage_JumpToSection(RepMontage.SectionName, RepMontage.CurrentMontage);
		}
		UE_LOG(AbilityFramework, Log, TEXT("OnRep_PlayMontage MontageName: %s SectionNAme: %s ForceRep: %s"), *RepMontage.CurrentMontage->GetName(), *RepMontage.SectionName.ToString(), *FString::FormatAsNumber(RepMontage.ForceRep)); 
	}
}

void UAFAbilityComponent::PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed)
{
	//if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		//Probabaly want to do something different here for client non authority montage plays.
		//return;
	}
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(MontageIn, Speed);
		if (SectionName != NAME_None)
		{
			//AnimInst->Montage_JumpToSection(SectionName, MontageIn);
		}

		UE_LOG(AbilityFramework, Log, TEXT("PlayMontage MontageName: %s SectionNAme: %s Where: %s"), *MontageIn->GetName(), *SectionName.ToString(), (GetOwnerRole() < ENetRole::ROLE_Authority ? TEXT("Client") : TEXT("Server")));
		RepMontage.SectionName = SectionName;
		RepMontage.CurrentMontage = MontageIn;
		RepMontage.ForceRep++;
	}
}
void UAFAbilityComponent::MulticastPlayMontage_Implementation(UAnimMontage* MontageIn, FName SectionName, float Speed = 1)
{

}