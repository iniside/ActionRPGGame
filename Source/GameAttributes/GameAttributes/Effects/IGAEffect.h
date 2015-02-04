#pragma once
#include "IGAEffect.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEATTRIBUTES_API UIGAEffect : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGAEffect
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		virtual class UGAEffectComponent* GetEffectComponent() = 0;
};