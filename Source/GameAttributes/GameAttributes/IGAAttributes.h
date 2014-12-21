#pragma once
#include "IGAAttributes.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEATTRIBUTES_API UIGAAttributes : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGAAttributes
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() = 0;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributeComponent* GetAttributeComponent() = 0;
};