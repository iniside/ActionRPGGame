// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityInterface.h"
#include "AFEffectsComponent.h"
#include "AFEffectTask_EffectAppliedToTarget.h"




UAFEffectTask_EffectAppliedToTarget::UAFEffectTask_EffectAppliedToTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_EffectAppliedToTarget* UAFEffectTask_EffectAppliedToTarget::ListenEffectAppliedToTarget(UGAEffectExtension* OwningExtension, FName TaskName, AActor* OptionalExternalTarget, bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_EffectAppliedToTarget>(OwningExtension, TaskName);
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	
	return MyObj;
}

void UAFEffectTask_EffectAppliedToTarget::Activate()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		//(this, &UAFEffectTask_EffectAppliedToTarget::GameplayEventCallback
		//FAFEventDelegate Delegate = FAFEventDelegate::CreateUObject(this, &UAFEffectTask_EffectAppliedToTarget::GameplayEventCallback);
		//MyHandle = Delegate.GetHandle();
		MyHandle = ASC->OnAppliedToTarget.AddUObject(this, &UAFEffectTask_EffectAppliedToTarget::GameplayEventCallback);//AddExecuteEvent(Tag, Delegate);
	}

	Super::Activate();
}

void UAFEffectTask_EffectAppliedToTarget::GameplayEventCallback(FGAEffectContext Context
	, FAFPropertytHandle Property
	, FAFEffectSpec Spec)
{
	//if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnEvent.Broadcast(Context, Property, Spec);
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAFEffectTask_EffectAppliedToTarget::OnTaskEnded()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
		ASC->OnAppliedToTarget.Remove(MyHandle);
	}
}

void UAFEffectTask_EffectAppliedToTarget::SetExternalTarget(AActor* Actor)
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

UAFEffectsComponent* UAFEffectTask_EffectAppliedToTarget::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return EffectsComponent;
}
