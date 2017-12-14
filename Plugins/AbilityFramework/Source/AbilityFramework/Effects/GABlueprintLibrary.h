#pragma once
#include "../Effects/GAGameEffect.h"
#include "../GAGlobalTypes.h"
#include "../Attributes/GAAttributeBase.h"
#include "GAEffectGlobalTypes.h"
#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ABILITYFRAMEWORK_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToObject(UPARAM(ref) FGAEffectProperty& InEffect,
			class UObject* Target, class APawn* Instigator,
			UObject* Causer, const FAFFunctionModifier& Modifier);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToActor(UPARAM(ref) FGAEffectProperty& InEffect,
			class AActor* Target, class APawn* Instigator,
			UObject* Causer, const FAFFunctionModifier& Modifier);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static FGAEffectHandle ApplyGameEffectToLocation(UPARAM(ref) FGAEffectProperty& InEffect,
			const FHitResult& Target, class APawn* Instigator,
			UObject* Causer, const FAFFunctionModifier& Modifier);

	static FGAEffectHandle ApplyEffect(FGAEffectProperty& InEffect,
		class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FHitResult& HitIn, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	static FGAEffectHandle ApplyEffect(FGAEffectProperty* InEffect,
		class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FHitResult& HitIn, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	static FGAEffectHandle ApplyEffectFromHit(FGAEffectProperty& InEffect,
		const FHitResult& Target, class APawn* Instigator,
		UObject* Causer, const FAFFunctionModifier& Modifier);
	
	static FGAEffectHandle ApplyEffectToActor(FGAEffectProperty& InEffect,
		 class AActor* Target, class APawn* Instigator,
		UObject* Causer, const FAFFunctionModifier& Modifier);

	static FGAEffectHandle ApplyEffectToObject(FGAEffectProperty& InEffect,
		class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FAFFunctionModifier& Modifier);
	/*
		Create Effect but does not apply it.
	*/
	static FGAEffectHandle MakeEffect(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FHitResult& HitIn);


	static FGAEffectContext MakeContext(class UObject* Target, class APawn* Instigator, AActor* InAvatar, 
		UObject* Causer, const FHitResult& HitIn);
	static void AddTagsToEffect(FGAEffect* EffectIn);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static FGAEffectContext& GetContext(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static UAFAbilityComponent* GetTargetComponent(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Effects")
	static UAFAbilityComponent* GetInstigatorComponent(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Effects")
		static void BroadcastEffectEvent(UObject* Target, FGameplayTag EventTag);
};
