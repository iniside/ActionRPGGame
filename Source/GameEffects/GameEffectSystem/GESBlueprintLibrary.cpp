// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffectComponent.h"
#include "GESEffect.h"
#include "IGESEffect.h"
#include "GESBlueprintLibrary.h"

UGESBlueprintLibrary::UGESBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UGESEffect* UGESBlueprintLibrary::CreateEffect(TSubclassOf<class UGESEffect> EffectClass, AActor* Causer, AActor* Target, AActor* Instigator)
{
	UGESEffect* effectOut = nullptr;
	if (EffectClass && Target && Instigator && Causer)
	{
		IIGESEffect* targetInt = Cast<IIGESEffect>(Target);
		IIGESEffect* instigatorInt = Cast<IIGESEffect>(Instigator);
		if (!targetInt && !instigatorInt)
			return effectOut;

		UGESEffectComponent* targetComp = targetInt->GetEffectComponent();
		UGESEffectComponent* instigComp = instigatorInt->GetEffectComponent();
		effectOut = ConstructObject<UGESEffect>(EffectClass);

		effectOut->Target = Target;
		effectOut->Causer = Causer;
		effectOut->Instigator = Instigator;
		effectOut->InstigatorEffectComponent = instigComp;
		effectOut->TargetEffectComponent = targetComp;
		effectOut->Initialize();
		//instigComp->ApplyEffectToTarget(effectOut, Target);
		//instigComp->AddPeriodicEffect(effectOut);
	}

	return effectOut;
}

void UGESBlueprintLibrary::ApplyEffect(class UGESEffect* EffectIn)
{
	if (!EffectIn)
		return;

	EffectIn->InstigatorEffectComponent->ApplyEffectToTarget(EffectIn, EffectIn->Target);
}