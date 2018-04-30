// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityInterface.h"
#include "AFEffectsComponent.h"
#include "AFEffectTask_AppliedEffectEvent.h"




UAFEffectTask_AppliedEffectEvent::UAFEffectTask_AppliedEffectEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_AppliedEffectEvent* UAFEffectTask_AppliedEffectEvent::ListenAppliedEffectEvent(UGAEffectExtension* OwningExtension, FName TaskName, FGameplayTag Tag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_AppliedEffectEvent>(OwningExtension, TaskName);
	MyObj->Tag = Tag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	
	return MyObj;
}

void UAFEffectTask_AppliedEffectEvent::Activate()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		//(this, &UAFEffectTask_AppliedEffectEvent::GameplayEventCallback
		FAFEventDelegate Delegate = FAFEventDelegate::CreateUObject(this, &UAFEffectTask_AppliedEffectEvent::GameplayEventCallback);
		MyHandle = Delegate.GetHandle();
		ASC->AddAppliedEvent(Tag, Delegate);
	}

	Super::Activate();
}

void UAFEffectTask_AppliedEffectEvent::GameplayEventCallback(FAFEventData Payload)
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
void UAFEffectTask_AppliedEffectEvent::OnTaskEnded()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		ASC->RemoveAppliedEvent(Tag, MyHandle);
	}
}
void UAFEffectTask_AppliedEffectEvent::SetExternalTarget(AActor* Actor)
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

UAFEffectsComponent* UAFEffectTask_AppliedEffectEvent::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return EffectsComponent;
}