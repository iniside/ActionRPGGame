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
UAFEffectsComponent::UAFEffectsComponent()
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

FGAEffectHandle UAFEffectsComponent::ApplyEffectToSelf(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty
	, FGAEffectContext& InContext
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
	if (FAFEffectEvent* Delegate = OnEffectApplyToSelf.Find(InProperty.GetSpec()->EffectTag))
	{
		Delegate->ExecuteIfBound();
	}

	FGAEffectHandle Handle = GameEffectContainer.ApplyEffect(EffectIn, InProperty, InContext, Modifier);
	GameEffectContainer.MarkArrayDirty();
	return Handle;
}

FGAEffectHandle UAFEffectsComponent::ApplyEffectToTarget(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty
	, FGAEffectContext& InContext
	, const FAFFunctionModifier& Modifier)
{
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
		Handle = InContext.GetTargetEffectsComponent()->ApplyEffectToSelf(EffectIn, InProperty, InContext, Modifier);
		if (!PropertyByHandle.Contains(Handle))
			PropertyByHandle.Add(Handle, &InProperty);

		EffectToCue.Add(Handle, CueHandle);
		
		return Handle;
	}
	return FGAEffectHandle();
}


/* Have to to copy handle around, because timer delegates do not support references. */
void UAFEffectsComponent::ExecuteEffect(FGAEffectHandle HandleIn
	, FGAEffectProperty InProperty
	, FAFFunctionModifier Modifier
	, FGAEffectContext InContext)
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
	FGAEffect& Effect = HandleIn.GetEffect();
	FGAEffectMod Mod = FAFStatics::GetAttributeModifier(InProperty.GetAttributeModifier()
		, InProperty.GetSpec()
		, InContext
		, HandleIn);

	//execute period regardless if this periodic effect ? Or maybe change name OnEffectExecuted ?

	Effect.OnExecuted();
	ExecuteEffectEvent(InProperty.GetSpec()->OnPeriodEvent);

	FAFCueHandle* CueHandle = EffectToCue.Find(HandleIn);
	if (CueHandle)
		MulticastExecuteEffectCue(*CueHandle);

	InProperty.EffectExecution(HandleIn, Mod, HandleIn.GetContextRef(), Modifier);
	PostExecuteEffect();
}

void UAFEffectsComponent::PostExecuteEffect()
{

}
/* ExpireEffect is used to remove existing effect naturally when their time expires. */
void UAFEffectsComponent::ExpireEffect(FGAEffectHandle HandleIn
	, FGAEffectProperty InProperty
	, FGAEffectContext InContext)
{
	//call effect internal delegate:
	HandleIn.GetEffectPtr()->OnExpired();
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

	TArray<FGAEffectHandle> handles = GameEffectContainer.RemoveEffect(InProperty);
}

void UAFEffectsComponent::ClientExpireEffect_Implementation(FAFPredictionHandle PredictionHandle)
{

}

void UAFEffectsComponent::RemoveEffect(const FGAEffectProperty& InProperty
	, const FGAEffectContext& InContext
	, const FGAEffectHandle& InHandle)
{
	//if (GetOwnerRole() == ENetRole::ROLE_Authority
	//	|| GetOwner()->GetNetMode() == ENetMode::NM_Standalone)
	{
		InternalRemoveEffect(InProperty, InContext);
	}
	ExecuteEffectEvent(InProperty.GetSpec()->OnRemovedEvent);
	FGAEffectCueParams CueParams(InContext, InProperty);

	FAFCueHandle* CueHandle = EffectToCue.Find(InHandle);
	if (CueHandle)
	{
		MulticastRemoveEffectCue(CueParams, *CueHandle);
	}
}
void UAFEffectsComponent::InternalRemoveEffect(const FGAEffectProperty& InProperty, const FGAEffectContext& InContext)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("UAFAbilityComponent:: Reset Timers and Remove Effect"));

	//MulticastRemoveEffectCue(HandleIn);
	if (InProperty.GetSpec()->Cues.CueTags.Num() > 0)
	{
		FGAEffectCueParams CueParams(InContext, InProperty);
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

const TSet<FGAEffectHandle>& UAFEffectsComponent::GetAllEffectsHandles() const
{
	return GameEffectContainer.GetAllEffectHandles();
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
FAFEventDelegate& UAFEffectsComponent::GetTagEvent(FGameplayTag TagIn)
{
	FAFEventDelegate& Delegate = EffectEvents.FindChecked(TagIn);
	return Delegate;
}

void UAFEffectsComponent::NativeTriggerTagEvent(FGameplayTag TagIn, const FAFEventData& InEventData)
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
		UE_LOG(AbilityFramework, Log, TEXT("%s : MulticastApplyEffectCue Owner: %s, Instigator: %s"),
			*prefix,
			*GetOwner()->GetName(),
			*CueParams.Instigator->GetName()
		);

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
}

void UAFEffectsComponent::OnRep_GameEffectContainer()
{

}

/*Network Functions - END */