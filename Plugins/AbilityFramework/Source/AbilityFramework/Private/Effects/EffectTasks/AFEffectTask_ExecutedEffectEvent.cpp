// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityInterface.h"
#include "AFEffectsComponent.h"
#include "AFEffectTask_ExecutedEffectEvent.h"




UAFEffectTask_ExecutedEffectEvent::UAFEffectTask_ExecutedEffectEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_ExecutedEffectEvent* UAFEffectTask_ExecutedEffectEvent::ListenExecutedEffectEvent(UGAEffectExtension* OwningExtension, FName TaskName, FGameplayTag Tag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_ExecutedEffectEvent>(OwningExtension, TaskName);
	MyObj->Tag = Tag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	
	return MyObj;
}

void UAFEffectTask_ExecutedEffectEvent::Activate()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		//(this, &UAFEffectTask_ExecutedEffectEvent::GameplayEventCallback
		FAFEventDelegate Delegate = FAFEventDelegate::CreateUObject(this, &UAFEffectTask_ExecutedEffectEvent::GameplayEventCallback);
		MyHandle = Delegate.GetHandle();
		ASC->AddExecuteEvent(Tag, Delegate);
	}

	Super::Activate();
}

void UAFEffectTask_ExecutedEffectEvent::GameplayEventCallback(FAFEventData Payload)
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

void UAFEffectTask_ExecutedEffectEvent::OnTaskEnded()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		ASC->RemoveExecuteEvent(Tag, MyHandle);
	}
}

void UAFEffectTask_ExecutedEffectEvent::SetExternalTarget(AActor* Actor)
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

UAFEffectsComponent* UAFEffectTask_ExecutedEffectEvent::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return EffectsComponent;
}