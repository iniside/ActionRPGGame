#pragma once
#include "../GAGlobalTypes.h"
#include "../GAAttributeBase.h"
#include "GABlueprintLibrary.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyEffectSpec(const FHitResult& Target, APawn* Instigator,
		UObject* Causer, TSubclassOf<class UGAEffectSpecification> SpecIn);

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		static FGAEffectHandle ApplyEffectActorSpec(AActor* Target, APawn* Instigator,
		UObject* Causer, TSubclassOf<class UGAEffectSpecification> SpecIn);

};
