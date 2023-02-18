// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectTask_AttributeChange.h"


UAFEffectTask_AttributeChange::UAFEffectTask_AttributeChange(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_AttributeChange* UAFEffectTask_AttributeChange::ListenAttributeChanged(UGAEffectExtension* OwningExtension,
	FGAAttribute InAttribute,
	AActor* OptionalExternalTarget, 
	bool OnlyTriggerOnce)
{
	auto MyObj = NewEffectTask<UAFEffectTask_AttributeChange>(OwningExtension);
	MyObj->Attribute = InAttribute;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;

	return MyObj;
}

void UAFEffectTask_AttributeChange::Activate()
{
	UAFEffectsComponent* ASC = GetTargetASC();
	if (ASC)
	{
	//	MyHandle = ASC->AttributeChanged.FindOrAdd(Attribute).AddUObject(this, &UAFEffectTask_AttributeChange::AttributeChangedCallback);
	}

	Super::Activate();
}

void UAFEffectTask_AttributeChange::AttributeChangedCallback(FAFAttributeChangedData Payload)
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

void UAFEffectTask_AttributeChange::SetExternalTarget(AActor* Actor)
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

UAFEffectsComponent* UAFEffectTask_AttributeChange::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return EffectsComponent;
}