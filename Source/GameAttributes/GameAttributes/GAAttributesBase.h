#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributesBase.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEATTRIBUTES_API UGAAttributesBase : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	~UGAAttributesBase();

	virtual UProperty* FindProperty(FGAAttribute AttributeIn);

	/*
		This is proof of concept. Instead of typing name manually I want to display list of all
		properties from currently referenced object. 
		From that list user can select attribute which can be affected.

		I will also probabaly wil custom meta like GameAttribute = true, to filter trough
		other possibly non-attribute related properties.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetFloatValue(FGAAttribute AttributeIn);

	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	void SetNetAddressable();
protected:
	bool bNetAddressable;

private:
	//attribute caching
	//UPROPERTY() - propabaly don't need to be uproperty anyway.
	UProperty* LastAttributeProp;
	FName LastAttributeName;
	// cached numeric property. WEll we probabaly don't need UProperty cache then..
	UNumericProperty* CachedFloatPropety;

};
