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
		static TArray<FGAEffectHandle> ApplyGameEffectToObject(
			UPARAM(Ref) FAFPropertytHandle& InEffect
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
		static TArray<FGAEffectHandle> ApplyGameEffectToActor(
			UPARAM(Ref) FAFPropertytHandle& InEffect
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
		static TArray<FGAEffectHandle> ApplyGameEffectToLocation(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, const FHitResult& Target
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);


	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static TArray<FGAEffectHandle> ApplyGameEffectToObjects(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, TArray<class UObject*> Targets
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static TArray<FGAEffectHandle> ApplyGameEffectToActors(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, TArray<class AActor*> Targets
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);

	/*
	Makes outgoing effect spec and assign handle to it.
	If valid handle is provided it will instead reuse existing effect spec from handle,
	and just change context of effect.
	*/

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static TArray<FGAEffectHandle> ApplyGameEffectToTargets(
			UPARAM(Ref) FAFPropertytHandle& InEffect
			, const TArray<FHitResult>& Targets
			, class APawn* Instigator
			, UObject* Causer
			, const FAFFunctionModifier& Modifier);


	static TArray<FGAEffectHandle> ApplyEffect(
		FAFPropertytHandle& InEffect
		, TArray<class UObject*> Targets
		, class APawn* Instigator
		, UObject* Causer
		, const TArray<FHitResult>& HitsIn
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());
	
	static TArray<FGAEffectHandle> ApplyEffectFromHit(
		FAFPropertytHandle& InEffect
		, const TArray<FHitResult>& Targets
		, class APawn* Instigator
		, UObject* Causer
		, const FAFFunctionModifier& Modifier);
	
	static TArray<FGAEffectHandle> ApplyEffectToActor(
		FAFPropertytHandle& InEffect
		, TArray<class AActor*> Targets
		, class APawn* Instigator
		, UObject* Causer
		, const FAFFunctionModifier& Modifier);

	static TArray<FGAEffectHandle> ApplyEffectToObject(
		FAFPropertytHandle& InEffect
		, TArray<class UObject*> Targets
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
