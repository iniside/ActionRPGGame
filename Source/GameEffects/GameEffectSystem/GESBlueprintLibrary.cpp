// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffectComponent.h"
#include "GESEffect.h"
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
		UGESEffectComponent* targetComp = Target->FindComponentByClass<UGESEffectComponent>();
		UGESEffectComponent* instigComp = Instigator->FindComponentByClass<UGESEffectComponent>();
		effectOut = ConstructObject<UGESEffect>(EffectClass);

		effectOut->Target = Target;
		effectOut->Causer = Causer;
		effectOut->Instigator = Instigator;
		effectOut->OutgoingEffectComponent = instigComp;
		effectOut->IncomingEffectComponent = targetComp;
		effectOut->Initialize();
		instigComp->ApplyEffectToTarget(effectOut, Target);
		//instigComp->AddPeriodicEffect(effectOut);
	}

	return effectOut;
}