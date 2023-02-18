#pragma once
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "AFBlueprintFunctionLibrary.generated.h"
/*
	Some static helper functions, to interact with Attribute system.
*/
UCLASS()
class ABILITYFRAMEWORK_API UAFBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		static void TriggerAbilityPressedByTag(UObject* Target, const FGameplayTag& AbilityTag, FGameplayTag ActionTag);
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		static void TriggerAbilityReleasedByTag(UObject* Target, const FGameplayTag& AbilityTag, FGameplayTag ActionTag);
};
