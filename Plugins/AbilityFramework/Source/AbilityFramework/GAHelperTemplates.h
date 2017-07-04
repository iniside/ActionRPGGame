// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/NetSerialization.h"
#include "GameplayTags.h"
#include "AFAbilityComponent.h"
#include "GAHelperTemplates.generated.h"

USTRUCT(BlueprintType)
struct FGAAttributeSource
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		EGAAttributeSource Source;
	/* Class which is source for attribute set. */
	UPROPERTY(EditAnywhere)//, meta = (MustImplement = "IGAAbilities"))
		FGameplayTag AttributeSource;
};

USTRUCT(BlueprintType)
struct FGAAttributeCapture
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGAAttributeSource Source;

	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer DenyTags;

	template <class T>
	T* GetAttributeSet(const FGAEffectContext& ContextIn)
	{
		UAFAbilityComponent* TargetComp = ContextIn.TargetComp.Get();
		UAFAbilityComponent* InstigatorComp = ContextIn.InstigatorComp.Get();
		T* AttributeSet = nullptr;
		switch (Source.Source)
		{
		case EGAAttributeSource::Causer:
		{
			break;
		}
		case EGAAttributeSource::Target:
		{
			AttributeSet = TargetComp->GetAttributeSet<T>(Source.AttributeSource);
			break;
		}
		case EGAAttributeSource::Instigator:
		{
			AttributeSet = InstigatorComp->GetAttributeSet<T>(Source.AttributeSource);
			break;
		}
		}
		return AttributeSet;
	}
};