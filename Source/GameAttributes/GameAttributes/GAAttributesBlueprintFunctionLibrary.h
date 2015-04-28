#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributesBlueprintFunctionLibrary.generated.h"
/*
	Some static helper functions, to interact with Attribute system.
*/
UCLASS()
class GAMEATTRIBUTES_API UGAAttributesBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Equal"), Category = "GameAttributes")
		static bool EqualAttribute(const FGAAttribute& Compare, FGAAttribute Against);

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		static FName GetAttribute(FGAAttribute AttributeIn);

	UFUNCTION(BlueprintPure, Category = "Game Attributes")
		static float GetFinalAttributeValue(AActor* Target, FGAAttribute Name);

	UFUNCTION(BlueprintPure, Category = "Game Attributes")
		static float GetCurrentAttributeValue(AActor* Target, FGAAttribute Name);

	/**
	*	Takes actor, and return value of specified attribute.
	*
	*	@param Target - Actrom from which take attribute
	*	@param AttributeIn - Attribute from which we want to get value
	*
	*  @return value of attribute from actor.
	*/
	UFUNCTION(BlueprintPure, Category = "Game Attributes")
		static float GetAttributeFloat(AActor* Target, FGAAttribute AttributeIn);
};
