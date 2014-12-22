#pragma once
#include "IGESEffectManager.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEEFFECTSYSTEM_API UIGESEffectManager : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};
/*
	Implement this interface on objects, from which you want to create new effects.
*/
class IIGESEffectManager
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual class UGESEffectManager* GetEffectManager() { return nullptr; };

	virtual class UGESEffect* GetCachedEffect() 
	{ 
		return nullptr;
	};

};