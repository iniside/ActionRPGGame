#pragma once
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAAttributesBlueprintFunctionLibrary.generated.h"
/*
	Some static helper functions, to interact with Attribute system.
*/
UCLASS()
class ABILITYFRAMEWORK_API UGAAttributesBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Equal"), Category = "AbilityFramework|Attributes")
		static bool EqualAttribute(const FGAAttribute& Compare, FGAAttribute Against);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		static FName GetAttribute(FGAAttribute AttributeIn);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Attributes")
		static float GetFinalAttributeValue(AActor* Target, FGAAttribute Name);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Attributes")
		static float GetCurrentAttributeValue(AActor* Target, FGAAttribute Name);

	/**
	*	Takes actor, and return value of specified attribute.
	*
	*	@param Target - Actrom from which take attribute
	*	@param AttributeIn - Attribute from which we want to get value
	*
	*  @return value of attribute from actor.
	*/
	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Attributes")
		static float GetAttributeFloat(AActor* Target, FGAAttribute AttributeIn);
	/**
	 *	Subtracts value specified by From effect
	 *  and adds it by effect specified by To.
	 *
	 *	@param Instigator - Who Insigated effects.
	 *	@param Causer - Who Caused Effects
	 *	@param From - effect which will subtract Value
	 *	@param FromTarget - from whose attributes value will be subtracted
	 *	@param To - Effect specifing where attribute should be added.
	 *	@param ToTarget - Target to which attributes will be added.
	 *
	 *  @return value of attribute from actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		static void ExchangeAttributesValues(
			APawn* Instigator
			, UObject* Causer
			, FAFPropertytHandle From
			, FGAEffectHandle FromHandle
			, UObject* FromTarget
			, FAFPropertytHandle To
			, FGAEffectHandle ToHandle
			, UObject* ToTarget);
};
