#pragma once
#include "../Effects/GAGameEffect.h"
#include "../GAGlobalTypes.h"
#include "../Attributes/GAAttributeBase.h"

#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ABILITYFRAMEWORK_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectProperty(UPARAM(ref) FGAEffectProperty& Handle);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffect(FGAEffectHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectToObject(UPARAM(ref) FGAEffectProperty& InEffect,
			class UObject* Target, class APawn* Instigator,
			UObject* Causer);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectToActor(UPARAM(ref) FGAEffectProperty& InEffect,
			class AActor* Target, class APawn* Instigator,
			UObject* Causer);
	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectToLocation(UPARAM(ref) FGAEffectProperty& InEffect,
			const FHitResult& Target, class APawn* Instigator,
			UObject* Causer);

	static FGAEffectHandle ApplyEffect(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FHitResult& HitIn);

	static FGAEffectHandle ApplyEffectFromHit(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
		UObject* Causer);
	
	static FGAEffectHandle ApplyEffectToActor(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
		UObject* Causer);

	static FGAEffectHandle ApplyEffectToObject(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
		UObject* Causer);
	/*
		Create Effect but does not apply it.
	*/
	static FGAEffectHandle MakeEffect(UGAGameEffectSpec* SpecIn,
		FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
		UObject* Causer, const FHitResult& HitIn);

	static FGAEffectHandle ApplyEffect(const FGAEffectHandle& HandleIn);

	static FGAEffectContext MakeContext(class UObject* Target, class APawn* Instigator, UObject* Causer, const FHitResult& HitIn);
	static void AddTagsToEffect(FGAEffect* EffectIn);

	UFUNCTION(BlueprintPure, Category = "Game Effect System")
	static FGAEffectContext& GetContext(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "Game Effect System")
	static UGAAbilitiesComponent* GetTargetComponent(const FGAEffectHandle& InHandle);

	UFUNCTION(BlueprintPure, Category = "Game Effect System")
	static UGAAbilitiesComponent* GetInstigatorComponent(const FGAEffectHandle& InHandle);
};
