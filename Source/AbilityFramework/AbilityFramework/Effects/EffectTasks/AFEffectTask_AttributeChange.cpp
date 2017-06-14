// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectTask_AttributeChange.h"


UAFEffectTask_AttributeChange::UAFEffectTask_AttributeChange(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UAFEffectTask_AttributeChange* UAFEffectTask_AttributeChange::ListenAttributeChanged(UObject* OwningExtension,
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
	UGAAbilitiesComponent* ASC = GetTargetASC();
	if (ASC)
	{
		MyHandle = ASC->AttributeChanged.FindOrAdd(Attribute).AddUObject(this, &UAFEffectTask_AttributeChange::AttributeChangedCallback);
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
		if (IIGAAbilities* interface = Cast<IIGAAbilities>(Actor))
		{
			UseExternalTarget = true;
			OptionalExternalTarget = interface->GetAbilityComp();
		}

	}
}

UGAAbilitiesComponent* UAFEffectTask_AttributeChange::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilityComponent;
}

void UAFEffectTask_AttributeChange::OnDestroy(bool AbilityEnding)
{
	UGAAbilitiesComponent* ASC = GetTargetASC();
	if (ASC && MyHandle.IsValid())
	{
		ASC->AttributeChanged.FindOrAdd(Attribute).Remove(MyHandle);
	}

	Super::OnDestroy(AbilityEnding);
}


