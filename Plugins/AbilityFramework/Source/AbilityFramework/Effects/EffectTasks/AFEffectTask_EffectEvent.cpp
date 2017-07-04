// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../../AFAbilityInterface.h"
#include "AFEffectTask_EffectEvent.h"




UAFEffectTask_EffectEvent::UAFEffectTask_EffectEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_EffectEvent* UAFEffectTask_EffectEvent::ListenEffectEvent(UObject* OwningExtension, FGameplayTag Tag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_EffectEvent>(OwningExtension);
	MyObj->Tag = Tag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;

	return MyObj;
}

void UAFEffectTask_EffectEvent::Activate()
{
	UAFAbilityComponent* ASC = GetTargetASC();
	if (ASC)
	{
		MyHandle = ASC->EffectEvents.FindOrAdd(Tag).AddUObject(this, &UAFEffectTask_EffectEvent::GameplayEventCallback);
	}

	Super::Activate();
}

void UAFEffectTask_EffectEvent::GameplayEventCallback(FAFEventData Payload)
{
	//if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnEvent.Broadcast(Payload);
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAFEffectTask_EffectEvent::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		
		if (IAFAbilityInterface* interface = Cast<IAFAbilityInterface>(Actor))
		{
			UseExternalTarget = true;
			OptionalExternalTarget = interface->GetAbilityComp();
		}
		
	}
}

UAFAbilityComponent* UAFEffectTask_EffectEvent::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilityComponent;
}

void UAFEffectTask_EffectEvent::OnDestroy(bool AbilityEnding)
{
	UAFAbilityComponent* ASC = GetTargetASC();
	if (ASC && MyHandle.IsValid())
	{
		ASC->EffectEvents.FindOrAdd(Tag).Remove(MyHandle);
	}

	Super::OnDestroy(AbilityEnding);
}
