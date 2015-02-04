#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributesBase.generated.h"


/*

	Execute effmod object linearlly in single iteration every time appropertiate attribute is modified.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEATTRIBUTES_API UGAAttributesBase : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	~UGAAttributesBase();

	virtual void InitializeAttributes();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Initialize Attributes"))
		bool BP_InitializeAttributes();
	/*
		Updates attributes.
		@param AttributeIn - attribute which changed value.

		You can use this function to update other attributes (for example derived ones),
		based on the primary attribute, if the primary attribute (AttributeIn), changed.

		I have yet to fully figure out how do I want it to work. But one thing for certain is
		that it must be fully functional from within blueprint.
	*/
	virtual void UpdateAttributes(const FGAAttribute& AttributeIn);
private:
	UProperty* FindProperty(const FGAAttribute& AttributeIn);

public:

	//UFUNCTION(BlueprintCallable, Category = "Game Attributes")

	/*
		Helper C++ functions. Shouldn't ever be exposed or called from blueprint. Also never
		try to override them.

		probabaly could also add support for int32 values.
	*/
	float GetFloatValue(const FGAAttribute& AttributeIn);
	float SetFloatValue(const FGAAttribute& AttributeIn, float ValueIn);
	float AttributeOperation(const FGAAttribute& AttributeIn, float ValueIn, EGAAttributeOp Operation);

	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	void SetNetAddressable();
protected:
	bool bNetAddressable;

private:
	UProperty* LastAttributeProp;
	FName LastAttributeName;
	// cached numeric property. WEll we probabaly don't need UProperty cache then..
	UNumericProperty* CachedFloatPropety;
	
	float AddAttributeFloat(float ValueA, float ValueB);
	float SubtractAttributeFloat(float ValueA, float ValueB);
	float MultiplyAttributeFloat(float ValueA, float ValueB);
	float DivideAttributeFloat(float ValueA, float ValueB);
};
