// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Effects/GAGameEffect.h"
#include "AFEffectSpecFunctionLibrary.generated.h"
UENUM(BlueprintType)
enum class EAFTagCompareResult : uint8
{
	Match,
	NoMatch,
};
UENUM()
enum class EAFTagContainerCompare : uint8
{
	HasAny,
	HasAnyExact,
	HasAll,
	HasAllExact,
};

UENUM()
enum class EAFTagCompare : uint8
{
	HasTag,
	HasTagExact
};


/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectSpecFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects|Spec")
		static void AppendOwnedTags(FAFEffectSpecHandle Spec, const FGameplayTagContainer& InTags);
	
	
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "Result"), Category = "AbilityFramework|Effects|Spec")
		static void CompareOwnedTags(FAFEffectSpecHandle Spec
			, EAFTagContainerCompare Mode
			, const FGameplayTagContainer& InTags
			, EAFTagCompareResult& Result);


	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "Result"), Category = "AbilityFramework|Effects|Spec")
		static void CompareOwnedTag(FAFEffectSpecHandle Spec
			, EAFTagCompare Mode
			, FGameplayTag InTag
			, EAFTagCompareResult& Result);

};
