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
};
