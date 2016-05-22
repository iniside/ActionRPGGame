#pragma once
#include "../GAGlobalTypes.h"
#include "../GAAttributeBase.h"
#include "../GAGameEffect.h"
#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="ResultOut"), Category = "Game Effect System")
		static FGAGameEffectHandle MakeOutgoingSpec(FGAGameEffectHandle Handle,
			TSubclassOf<class UGAGameEffectSpec> SpecIn, const FHitResult& Target, APawn* Instigator,
			UObject* Causer, EGAMakeSpecResult& ResultOut);

	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "ResultOut"), Category = "Game Effect System")
		static FGAGameEffectHandle MakeOutgoingSpecObj(FGAGameEffectHandle Handle,
			const FGAEffectSpec& SpecIn, const FHitResult& Target, APawn* Instigator,
			UObject* Causer, EGAMakeSpecResult& ResultOut);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffect(FGAGameEffectHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static void ApplyGameEffectInstance(TSubclassOf<class UGAEffectInstanced> EffectClass,
			const FHitResult& Target, APawn* Instigator,
			UObject* Causer, EGAMakeSpecResult& ResultOu);
	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAGameEffectHandle ApplyGameEffectToActor(const FGAEffectSpec& SpecIn,
			FGAGameEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
			UObject* Causer);
	/*
		Makes outgoing effect spec and assign handle to it.
		If valid handle is provided it will instead reuse existing effect spec from handle,
		and just change context of effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAGameEffectHandle ApplyGameEffectToActorFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
			FGAGameEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
			UObject* Causer);
	
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAGameEffectHandle ApplyGameEffectToLocation(const FGAEffectSpec& SpecIn,
			FGAGameEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
			UObject* Causer);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAGameEffectHandle ApplyGameEffectToLocationFromClass(TSubclassOf<class UGAGameEffectSpec> SpecIn,
			FGAGameEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
			UObject* Causer);

protected:
	static FGAGameEffectHandle ApplyEffectFromHit(UGAGameEffectSpec* SpecIn,
		FGAGameEffectHandle HandleIn, const FHitResult& Target, class APawn* Instigator,
		UObject* Causer);
	
	static FGAGameEffectHandle ApplyEffectToActor(UGAGameEffectSpec* SpecIn,
		FGAGameEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
		UObject* Causer);

	static FGAEffectContext MakeActorContext(class AActor* Target, class APawn* Instigator, UObject* Causer);
	static FGAEffectContext MakeHitContext(const FHitResult& Target, class APawn* Instigator, UObject* Causer);
	static void AddTagsToEffect(FGAGameEffect* EffectIn);
};
