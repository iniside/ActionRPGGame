// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectSpecFunctionLibrary.h"



void UAFEffectSpecFunctionLibrary::AppendOwnedTags(FAFEffectSpecHandle Spec, const FGameplayTagContainer& InTags)
{
	Spec.GetPtr()->OwnedTags.AppendTags(InTags);
}

void UAFEffectSpecFunctionLibrary::CompareOwnedTags(FAFEffectSpecHandle Spec
	, EAFTagContainerCompare Mode
	, const FGameplayTagContainer& InTags
	, EAFTagCompareResult& Result)
{
	switch (Mode)
	{
	case EAFTagContainerCompare::HasAny:
		Spec.GetPtr()->OwnedTags.HasAny(InTags) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
		break;
	case EAFTagContainerCompare::HasAnyExact:
		Spec.GetPtr()->OwnedTags.HasAnyExact(InTags) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
		break;
	case EAFTagContainerCompare::HasAll:
		Spec.GetPtr()->OwnedTags.HasAll(InTags) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
		break;
	case EAFTagContainerCompare::HasAllExact:
		Spec.GetPtr()->OwnedTags.HasAllExact(InTags) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
	default:
		break;
	}
}

void UAFEffectSpecFunctionLibrary::CompareOwnedTag(FAFEffectSpecHandle Spec
	, EAFTagCompare Mode
	, FGameplayTag InTag
	, EAFTagCompareResult& Result)
{
	switch (Mode)
	{
	case EAFTagCompare::HasTag:
		Spec.GetPtr()->OwnedTags.HasTag(InTag) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
		break;
	case EAFTagCompare::HasTagExact:
		Spec.GetPtr()->OwnedTags.HasTagExact(InTag) ? Result = EAFTagCompareResult::Match : Result = EAFTagCompareResult::NoMatch;
		break;
	}
}