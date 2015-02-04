// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffectComponent.h"
#include "GAEffect.h"
#include "IGAEffect.h"
#include "GABlueprintLibrary.h"

UGABlueprintLibrary::UGABlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


bool UGABlueprintLibrary::ApplyEffect(TSubclassOf<class UGAEffect> EffectClass, UObject* Causer, const FHitResult& Target, APawn* Instigator,
	float Magnitude, float Duration, int32 PeriodCount, const FGameplayTag& EffectTag,
	const FGameplayTagContainer& MagnitudeTags)
{
	bool bEffectAppiled = false;
	if (EffectClass && Target.Actor.IsValid() && Instigator && Causer)
	{
		IIGAEffect* targetInt = Cast<IIGAEffect>(Target.Actor.Get());
		IIGAEffect* instigatorInt = Cast<IIGAEffect>(Instigator);
		if (!targetInt || !instigatorInt)
			return bEffectAppiled;

		UGAEffectComponent* targetComp = targetInt->GetEffectComponent();
		UGAEffectComponent* instigComp = instigatorInt->GetEffectComponent();
		//effectOut = ConstructObject<UGESEffect>(EffectClass);
		FGAEffectSpec EffSpec;
		EffSpec.EffectClass = EffectClass;
		EffSpec.Target = Target;
		EffSpec.Causer = Causer;
		EffSpec.Instigator = Instigator;
		EffSpec.TargetEffectComp = targetComp;
		EffSpec.InstigatorEffectComp = instigComp;
		EffSpec.EffectTag = EffectTag;
		EffSpec.MagnitudeTags = MagnitudeTags;
		EffSpec.Magnitude = Magnitude;
		EffSpec.Duration = Duration;
		EffSpec.PeriodCount = PeriodCount;
		bEffectAppiled = instigComp->ApplyEffectToTarget(EffSpec, Target.Actor.Get());
		return bEffectAppiled;
	}
	return bEffectAppiled;
}

void UGABlueprintLibrary::RemoveEffects(AActor* CauserIn, AActor* TargetIn, const FGameplayTagContainer& EffectsTags, int32 EffectCount)
{
	IIGAEffect* targetInt = Cast<IIGAEffect>(TargetIn);
	IIGAEffect* causerInt = Cast<IIGAEffect>(CauserIn);
	if (!targetInt && !causerInt)
		return;

	causerInt->GetEffectComponent()->RemoveEffectFromTarget(TargetIn, EffectsTags, EffectCount);
}

bool UGABlueprintLibrary::HasAnyEffect(AActor* Target, const FGameplayTagContainer& Tags)
{
	return false;
}