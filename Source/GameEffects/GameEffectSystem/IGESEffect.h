#pragma once
#include "IGESEffect.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEEFFECTSYSTEM_API UIGESEffect : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGESEffect
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		virtual class UGESEffectComponent* GetEffectComponent() = 0;
};