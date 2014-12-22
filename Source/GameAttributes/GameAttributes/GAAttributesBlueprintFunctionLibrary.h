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
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")

	static FName GetAttribute(FGAAttribute AttributeIn);
	/**
	*	Takes actor, and return value of specified attribute.
	*
	*	@param Target - Actrom from which take attribute
	*	@param AttributeIn - Attribute from which we want to get value
	*
	*  @return value of attribute from actor.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		static float GetAttributeFloat(AActor* Target, FGAAttribute AttributeIn);
	/**
	 *	Takes attribute, change it and then return it's new value.
	 *	It always Work Like Attribute + ValueIn, Attribute - ValueIn, Attribute \ ValueIn.
	 *
	 *	@param Target - Actor on which to change attribute
	 *	@param AttributeIn - Attribute, To change
	 *	@param ValueIn - Value by which change attribute
	 *	@param Operation - type of operation to perform on attribute
	 *
	 *  @return value of changed attribute
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		static float ChangeAttribute(AActor* Target, FGAAttribute AttributeIn, float ValueIn, EGAAttributeOp Operation, bool bSetAttribute);
	/**
	*	Takes array attributes, change them;
	*	It always Work Like Attribute + ValueIn, Attribute - ValueIn, Attribute \ ValueIn.
	*
	*	@param Target - Actor on which to change attributes
	*	@param Attributes - Attributes to change.
	*
	*   
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		static void ModifyAttributes(TArray<FGAAttributeModifier> AttributesIn);
};
