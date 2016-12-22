#pragma once
#include "../GAGameEffect.h"
#include "../GAGlobalTypes.h"
#include "../GAAttributeBase.h"

#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEABILITIES_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffect(FGAEffectHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectInstance(TSubclassOf<class UGAEffectExtension> EffectClass,
			const FHitResult& Target, APawn* Instigator,
			UObject* Causer, EGAMakeSpecResult& ResultOu);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToObject(const FGAEffectSpec& SpecIn,
			FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
			UObject* Causer);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToObjectFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
			FGAEffectHandle HandleIn, class UObject* Target, class APawn* Instigator,
			UObject* Causer);

	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToActor(const FGAEffectSpec& SpecIn,
			FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
			UObject* Causer);
	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToActorFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
			FGAEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
			UObject* Causer);
	
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToLocation(const FGAEffectSpec& SpecIn,
			FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
			UObject* Causer);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyGameEffectToLocationFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
			FGAEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
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
};
