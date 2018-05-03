#pragma once
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "Attributes/GAAttributeBase.h"
#include "GAEffectGlobalTypes.h"
#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ABILITYFRAMEWORK_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToObject(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, FGAEffectHandle Handle
			, class UObject* Target
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToActor(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, FGAEffectHandle Handle
			, class AActor* Target
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToLocation(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, FGAEffectHandle Handle
			, const FHitResult& Target
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);

	static FGAEffectHandle ApplyEffect(
		FAFPropertytHandle& InEffect
		, const FGAEffectHandle& Handle
		, class UObject* Target
		, class APawn* Instigator
		, UObject* Causer
		, const FHitResult& HitIn
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	static FGAEffectHandle ApplyEffect(
		FGAEffectProperty* InEffect
		, const FGAEffectHandle& Handle
		, class UObject* Target
		, class APawn* Instigator
		, UObject* Causer
		, const FHitResult& HitIn
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	static FGAEffectHandle ApplyEffectFromHit(
		FAFPropertytHandle& InEffect
		, const FGAEffectHandle& Handle
		, const FHitResult& Target
		, class APawn* Instigator
		, UObject* Causer
		, const FAFFunctionModifier& Modifier);
	
	static FGAEffectHandle ApplyEffectToActor(
		FAFPropertytHandle& InEffect
		, const FGAEffectHandle& Handle
		, class AActor* Target
		, class APawn* Instigator
		, UObject* Causer
		, const FAFFunctionModifier& Modifier);

	static FGAEffectHandle ApplyEffectToObject(
		FAFPropertytHandle& InEffect
		, const FGAEffectHandle& Handle
		, class UObject* Target
		, class APawn* Instigator
		, UObject* Causer
		, const FAFFunctionModifier& Modifier);
	/*
		Create Effect but does not apply it.
	*/

	static FAFContextHandle MakeContext(class UObject* Target, class APawn* Instigator, AActor* InAvatar,
		UObject* Causer, const FHitResult& HitIn);
	static void AddTagsToEffect(FAFEffectSpec* EffectIn);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static FGAEffectContext& GetContext(const FAFContextHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static UAFAbilityComponent* GetTargetComponent(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static UAFAbilityComponent* GetInstigatorComponent(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static void BroadcastEffectEvent(UObject* Target, FGameplayTag EventTag);


	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static void CreateEffectSpec(UPARAM(Ref) FAFEffectSpecHandle& InOutSpec
			, const FAFPropertytHandle& InEffect
			, class UObject* Target
			, class APawn* Instigator
			, UObject* Causer);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static void ApplyEffectFromSpec(UPARAM(Ref) FAFPropertytHandle& InEffect, UPARAM(Ref) FAFEffectSpecHandle& InSpec);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static void ModifyAttributeSimple(
			UPARAM(Ref) FAFEffectSpecHandle& InSpec
			, UObject* Target);
};
