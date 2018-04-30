// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectsComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "AFAbilityInterface.h"
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"
#include "Effects/GAEffectExtension.h"
#include "Effects/GAEffectCue.h"
#include "AFCueManager.h"
#include "Effects/GABlueprintLibrary.h"

// Sets default values for this component's properties
UAFEffectsComponent::UAFEffectsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAFEffectsComponent::BeginPlay()
{
	Super::BeginPlay();
	GameEffectContainer.OwningComponent = this;
	// ...
	
}
void UAFEffectsComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GameEffectContainer.OwningComponent = this;
}

// Called every frame
void UAFEffectsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FGAEffectHandle UAFEffectsComponent::ApplyEffectToSelf(
	const FGAEffect& EffectIn
	, const FGAEffectHandle& InHandle
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	FGAEffectProperty& InProperty = Params.GetProperty();
	const FGameplayTagContainer& AppliedEventTags = InProperty.GetSpec()->AppliedEventTags;
	
	FAFEventData Data;

	for (const FGameplayTag& Tag : AppliedEventTags)
	{
		if (TArray<FAFEventDelegate>* Events = EffectEvents.Find(Tag))
		{
			for (const FAFEventDelegate& Event : *Events)
				Event.Execute(Data);
		}
		
	}
	OnAppliedToSelf.Broadcast(Params.Context, Params.Property, Params.EffectSpec);
	
	if (FAFEffectEvent* Delegate = OnEffectApplyToSelf.Find(InProperty.GetSpec()->EffectTag))
	{
		Delegate->ExecuteIfBound();
	}

	FGAEffectHandle Handle = GameEffectContainer.ApplyEffect(EffectIn, InHandle, Params, Modifier);
	GameEffectContainer.MarkArrayDirty();
	return Handle;
}

FGAEffectHandle UAFEffectsComponent::ApplyEffectToTarget(
	const FGAEffect& EffectIn
	, const FGAEffectHandle& InHandle
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	FGAEffectProperty& InProperty = Params.GetProperty();
	FGAEffectContext& InContext = Params.GetContext();
	if (FAFEffectEvent* Delegate = OnEffectApplyToTarget.Find(InProperty.GetSpec()->EffectTag))
	{
		Delegate->ExecuteIfBound();
	}
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();

	FAFCueHandle CueHandle = FAFCueHandle::GenerateHandle();

	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams(InContext, InProperty);
		MulticastApplyEffectCue(CueParams, CueHandle);
	}

	OnAppliedToTarget.Broadcast(Params.Context, Params.Property, Params.EffectSpec);

	if (InContext.TargetComp.IsValid())
	{
		FGAEffectHandle Handle;
		Handle = InContext.GetTargetEffectsComponent()->ApplyEffectToSelf(EffectIn, InHandle, Params, Modifier);
		//if (!PropertyByHandle.Contains(Handle))
		//	PropertyByHandle.Add(Handle, &InProperty);
		
		EffectToCue.Add(Handle, CueHandle);
		
		return Handle;
	}
	return FGAEffectHandle();
}


/* Have to to copy handle around, because timer delegates do not support references. */
void UAFEffectsComponent::ExecuteEffect(FGAEffectHandle HandleIn
	, FAFEffectParams Params
	, FAFFunctionModifier Modifier)
{
	FGAEffectContext& Context = Params.Context.GetRef();
	FGAEffectProperty& Property = Params.Property.GetRef();
	FAFEffectSpec& EffectSpec = Params.GetSpec();
	/*
	this patth will give effects chance to do any replicated events, like applying cues.
	WE do not make any replication at the ApplyEffect because some effect might want to apply cues
	on periods on expiration etc, and all those will go trouch ExecuteEffect path.
	*/
	const FGameplayTagContainer& RequiredTags = Property.GetSpec()->RequiredTags;
	if (RequiredTags.Num() > 0)
	{
		if (!HasAll(RequiredTags))
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Effect %s not executed, require tags: %s"), *Property.GetSpec()->GetName(), *RequiredTags.ToString());
			return;
		}
	}

	//apply execution events:
	const FGameplayTagContainer& ExecuteEventTags = Property.GetSpec()->ExecuteEventTags;
	FAFEventData Data(Params);
	for (const FGameplayTag& Tag : ExecuteEventTags)
	{
		TriggerExecuteEvent(Tag, Data);
	}

	//OnEffectExecuted.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Effect %s executed"), *Property.GetSpec()->GetName());
	
	FGAEffectMod Mod = FAFStatics::GetAttributeModifier(Property.GetAttributeModifier()
		, Property.GetSpec()
		, Context
		, HandleIn);

	EffectSpec.OnExecuted();
	ExecuteEffectEvent(Property.GetSpec()->OnPeriodEvent);

	FAFCueHandle* CueHandle = EffectToCue.Find(HandleIn);
	if (CueHandle)
		MulticastExecuteEffectCue(*CueHandle);

	Property.ExecuteEffect(HandleIn, Mod, Params, Modifier);
	PostExecuteEffect();
}

void UAFEffectsComponent::PostExecuteEffect()
{

}
/* ExpireEffect is used to remove existing effect naturally when their time expires. */
void UAFEffectsComponent::ExpireEffect(FGAEffectHandle HandleIn
	, FAFEffectParams Params)
{
	//call effect internal delegate:
	FGAEffectProperty& InProperty = Params.GetProperty();
	FGAEffectContext& InContext = Params.GetContext();
	FAFEffectSpec& EffectSpec = Params.GetSpec();
	FGAEffect* Effect = GameEffectContainer.GetEffect(HandleIn);
	EffectSpec.OnExpired();
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	ExecuteEffectEvent(InProperty.GetSpec()->OnExpiredEvent);
	if (mode == ENetMode::NM_DedicatedServer
		|| mode == ENetMode::NM_ListenServer)
	{
		ClientExpireEffect(InProperty.GetPredictionHandle());
	}

	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams(InContext, InProperty);

		FAFCueHandle* CueHandle = EffectToCue.Find(HandleIn);
		if (CueHandle)
		{
			MulticastRemoveEffectCue(CueParams, *CueHandle);
		}
	}

	TArray<FGAEffectHandle> handles = GameEffectContainer.RemoveEffect(Params.Property);
}

void UAFEffectsComponent::ClientExpireEffect_Implementation(FAFPredictionHandle PredictionHandle)
{

}

void UAFEffectsComponent::RemoveEffect(const FAFPropertytHandle& InProperty
	, const FGAEffectContext& InContext
	, const FGAEffectHandle& InHandle)
{
	if (!InProperty.IsValid())
	{
		UE_LOG(AFEffects, Log, TEXT("RemoveEffect - Trying to Remove invalid effect."));
		return;
	}
	//if (GetOwnerRole() == ENetRole::ROLE_Authority
	//	|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		InternalRemoveEffect(InProperty, InContext);
	}
	ExecuteEffectEvent(InProperty.GetSpec()->OnRemovedEvent);
	FGAEffectCueParams CueParams(InContext, InProperty.GetRef());

	FAFCueHandle* CueHandle = EffectToCue.Find(InHandle);
	if (CueHandle)
	{
		MulticastRemoveEffectCue(CueParams, *CueHandle);
	}
}
void UAFEffectsComponent::InternalRemoveEffect(const FAFPropertytHandle& InProperty, const FGAEffectContext& InContext)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Reset Timers and Remove Effect"));

	//MulticastRemoveEffectCue(HandleIn);
	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams(InContext, InProperty.GetRef());
		ENetRole role = GetOwnerRole();
		ENetMode mode = GetOwner()->GetNetMode();
		if (mode == ENetMode::NM_Standalone
			|| role >= ENetRole::ROLE_Authority)
		{
			//MulticastRemoveEffectCue(CueParams);
		}
	}

	TArray<FGAEffectHandle> handles = GameEffectContainer.RemoveEffect(InProperty);
}

void UAFEffectsComponent::AddEffectEvent(const FGameplayTag& InEventTag, const FSimpleDelegate& InEvent)
{
	if (!InEventTag.IsValid())
		return;
	if (!OnEffectEvent.Contains(InEventTag))
	{
		UE_LOG(AbilityFramework, Log, TEXT("AddEffectEvent: %s"), *InEventTag.ToString());
		OnEffectEvent.Add(InEventTag, InEvent);
	}
}
void UAFEffectsComponent::ExecuteEffectEvent(const FGameplayTag& InEventTag)
{
	if (!InEventTag.IsValid())
		return;
	FSimpleDelegate* Delegate = OnEffectEvent.Find(InEventTag);
	if (Delegate)
	{
		UE_LOG(AbilityFramework, Log, TEXT("ExecuteEffectEvent: %s"), *InEventTag.ToString());
		Delegate->ExecuteIfBound();
	}
}
void UAFEffectsComponent::RemoveEffectEvent(const FGameplayTag& InEventTag)
{
	if (!InEventTag.IsValid())
		return;
	UE_LOG(AbilityFramework, Log, TEXT("RemoveEffectEvent: %s"), *InEventTag.ToString());
	OnEffectEvent.Remove(InEventTag);
}
TArray<FAFEventDelegate>& UAFEffectsComponent::GetTagEvent(FGameplayTag TagIn)
{
	TArray<FAFEventDelegate>& Delegate = EffectEvents.FindChecked(TagIn);
	return Delegate;
}

void UAFEffectsComponent::NativeTriggerTagEvent(FGameplayTag TagIn, const FAFEventData& InEventData)
{
	TArray<FAFEventDelegate>* Delegate = EffectEvents.Find(TagIn);
	if (Delegate)
	{
		for (const FAFEventDelegate& Event : *Delegate)
		{
			if (Event.IsBound())
			{
				Event.Execute(InEventData);
			}
		}
	}
}

void UAFEffectsComponent::AddAppliedEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate)
{
	TArray<FAFEventDelegate>& Events = AppliedEvents.FindOrAdd(EventTag);
	Events.Add(EventDelegate);
}
void UAFEffectsComponent::RemoveAppliedEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate)
{
	TArray<FAFEventDelegate>* Events = AppliedEvents.Find(EventTag);
	if (Events)
	{
		int32 Idx = Events->IndexOfByPredicate([&](const FAFEventDelegate& Other)
		{
			return Other.GetHandle() == EventDelegate;
		});

		Events->RemoveAt(Idx, 1, true);
	}
}
void UAFEffectsComponent::TriggerAppliedEvent(FGameplayTag TagIn, const FAFEventData& InEventData)
{
	TArray<FAFEventDelegate>* Delegate = AppliedEvents.Find(TagIn);
	if (Delegate)
	{
		for (const FAFEventDelegate& Event : *Delegate)
		{
			if (Event.IsBound())
			{
				Event.Execute(InEventData);
			}
		}
	}
}

void UAFEffectsComponent::AddExecuteEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate)
{
	TArray<FAFEventDelegate>& Events = ExecutedEvents.FindOrAdd(EventTag);
	Events.Add(EventDelegate);
}
void UAFEffectsComponent::RemoveExecuteEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate)
{
	TArray<FAFEventDelegate>* Events = ExecutedEvents.Find(EventTag);
	if (Events)
	{
		int32 Idx = Events->IndexOfByPredicate([&](const FAFEventDelegate& Other)
		{
			return Other.GetHandle() == EventDelegate;
		});

		Events->RemoveAt(Idx, 1, true);
	}
}
void UAFEffectsComponent::TriggerExecuteEvent(FGameplayTag TagIn, const FAFEventData& InEventData)
{
	TArray<FAFEventDelegate>* Delegate = ExecutedEvents.Find(TagIn);
	if (Delegate)
	{
		for (const FAFEventDelegate& Event : *Delegate)
		{
			if (Event.IsBound())
			{
				Event.Execute(InEventData);
			}
		}
	}
}


void UAFEffectsComponent::AddEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate)
{
	TArray<FAFEventDelegate>& Events = EffectEvents.FindOrAdd(EventTag);
	Events.Add(EventDelegate);
}

void UAFEffectsComponent::RemoveEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate)
{
	TArray<FAFEventDelegate>* Events =  EffectEvents.Find(EventTag);
	if (Events)
	{
		
		int32 Idx = Events->IndexOfByPredicate([&](const FAFEventDelegate& Other)
		{
			return Other.GetHandle() == EventDelegate;
		});

		Events->RemoveAt(Idx, 1, true);
	}
}

bool UAFEffectsComponent::IsEffectActive(const FGAEffectHandle& InHandle) const
{
	return GameEffectContainer.IsEffectActive(InHandle);
}

bool UAFEffectsComponent::DenyEffectApplication(const FGameplayTagContainer& InTags)
{
	bool bDenyApplication = false;
	if (InTags.Num() > 0)
	{
		bDenyApplication = HasAll(InTags);
	}
	return bDenyApplication;
}

bool UAFEffectsComponent::HaveEffectRquiredTags(const FGameplayTagContainer& InTags)
{
	bool bAllowApplication = true;
	if (InTags.Num() > 0)
	{
		bAllowApplication = HasAll(InTags);
	}
	return bAllowApplication;
}

FGAEffect* UAFEffectsComponent::GetEffect(const FGAEffectHandle& InHandle)
{
	return *GameEffectContainer.ActiveEffects.Find(InHandle);
}

/*
Need prediction for spawning effects on client,
and then on updateing them predicitvely on all other clients.
*/
/*

*/
void UAFEffectsComponent::MulticastApplyEffectCue_Implementation(FGAEffectCueParams CueParams, FAFCueHandle InHandle)
{
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	if (mode == ENetMode::NM_Standalone
		|| role != ENetRole::ROLE_Authority)
	{
		UAFCueManager::Get()->HandleCue(CueParams.CueTags, CueParams, InHandle);
	}
}
void UAFEffectsComponent::MulticastExecuteEffectCue_Implementation(FAFCueHandle InHandle)
{
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	if (mode == ENetMode::NM_Standalone
		|| role != ENetRole::ROLE_Authority)
	{
		UAFCueManager::Get()->HandleExecuteCue(InHandle);
	}
}

void UAFEffectsComponent::MulticastRemoveEffectCue_Implementation(FGAEffectCueParams CueParams, FAFCueHandle InHandle)
{
	ENetRole role = GetOwnerRole();
	ENetMode mode = GetOwner()->GetNetMode();
	if (mode == ENetMode::NM_Standalone
		|| role != ENetRole::ROLE_Authority)
	{
		UAFCueManager::Get()->HandleRemoveCue(CueParams.CueTags, CueParams, InHandle);
	}
}

void UAFEffectsComponent::MulticastUpdateDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}

void UAFEffectsComponent::MulticastUpdatePeriodCue_Implementation(FGAEffectHandle EffectHandle, float NewPeriodIn)
{

}

void UAFEffectsComponent::MulticastUpdateTimersCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn)
{

}

void UAFEffectsComponent::MulticastExtendDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}


/*Network Functions - BEGIN */

void UAFEffectsComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UAFEffectsComponent, GameEffectContainer);
	DOREPLIFETIME(UAFEffectsComponent, AppliedTags);
	DOREPLIFETIME(UAFEffectsComponent, ImmunityTags);
}

void UAFEffectsComponent::OnRep_GameEffectContainer()
{

}

/*Network Functions - END */