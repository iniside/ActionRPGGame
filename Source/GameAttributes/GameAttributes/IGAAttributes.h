#pragma once
#include "GAGlobalTypes.h"
#include "IGAAttributes.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEATTRIBUTES_API UIGAAttributes : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class GAMEATTRIBUTES_API IIGAAttributes
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() = 0;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributeComponent* GetAttributeComponent() { return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const { return 0; };

	//UFUNCTION(BlueprintPure, Category = "Game Attributes")
	//	virtual float GetAttributeValuePure(FGAAttribute AttributeIn) const { return 0; };

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const { return 0; };

	//override to allow gathering tags from causer
	//those tags will be merged into effect owned tags.
	virtual FGameplayTagContainer GetCauserTags() { return FGameplayTagContainer(); }

	virtual void Died() {};
};