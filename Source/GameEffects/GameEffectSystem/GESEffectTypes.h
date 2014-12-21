#pragma once
#include "GESEffectTypes.generated.h"

USTRUCT()
struct FGESEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TWeakObjectPtr<AActor> Target;
	UPROPERTY()
		TWeakObjectPtr<AActor> Causer;
	UPROPERTY()
		TWeakObjectPtr<AActor> Instigator;

	UPROPERTY()
		TWeakObjectPtr<class UGESEffectComponent> TargetComponent;
	UPROPERTY()
		TWeakObjectPtr<class UGESEffectComponent> InstigatorComponent;

	UPROPERTY()
		TWeakObjectPtr<class UGESEffect> Effect;

	FGESEffectSpec();

	FGESEffectSpec(TWeakObjectPtr<AActor> TargetIn, TWeakObjectPtr<AActor> CauserIn, 
		TWeakObjectPtr<AActor> InstigatorIn);
};