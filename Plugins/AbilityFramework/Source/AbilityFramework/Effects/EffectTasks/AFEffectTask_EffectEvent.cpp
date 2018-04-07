// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../../AFAbilityInterface.h"
#include "AFEffectsComponent.h"
#include "AFEffectTask_EffectEvent.h"




UAFEffectTask_EffectEvent::UAFEffectTask_EffectEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_EffectEvent* UAFEffectTask_EffectEvent::ListenEffectEvent(UGAEffectExtension* OwningExtension, FName TaskName, FGameplayTag Tag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_EffectEvent>(OwningExtension, TaskName);
	MyObj->Tag = Tag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	
	return MyObj;
}

void UAFEffectTask_EffectEvent::Activate()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		//(this, &UAFEffectTask_EffectEvent::GameplayEventCallback
		FAFEventDelegate Delegate = FAFEventDelegate::CreateUObject(this, &UAFEffectTask_EffectEvent::GameplayEventCallback);
		MyHandle = Delegate.GetHandle();
		ASC->AddEvent(Tag, Delegate);
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

void UAFEffectTask_EffectEvent::OnTaskEnded()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		ASC->RemoveEvent(Tag, MyHandle);
	}
}

void UAFEffectTask_EffectEvent::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		
		if (IAFAbilityInterface* interface = Cast<IAFAbilityInterface>(Actor))
		{
			UseExternalTarget = true;
			OptionalExternalTarget = interface->GetEffectsComponent();
		}
		
	}
}

UAFEffectsComponent* UAFEffectTask_EffectEvent::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return EffectsComponent;
}