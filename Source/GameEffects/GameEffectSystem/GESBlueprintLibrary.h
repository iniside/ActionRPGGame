#pragma once
#include "GESBlueprintLibrary.generated.h"
/*
	Base class for effect.

	Effect is object, which modify actor with EffectComponent (or something like that).

	Effects can be divided into three basic categories:

	1. Instant - effect is appiled to actor, modify actor properties, and is deactivated and then
	garbage collected.
	2. Duration - effect is appiled to actor for set duration, after which is removed.
	3. Infinite - effect will exist for entire duration of game. (or actor).


	The main idea behind effects is to extend them trough blueprints, the base class shouldn't assume
	any gameplay type behaviours, only allow to easily create them trough blueprint.

	How exactly will that look, I'm not 100% sure.

	Effects should also be able to modify other effects.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEEFFECTSYSTEM_API UGESBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
	static class UGESEffect* CreateEffect(TSubclassOf<class UGESEffect> EffectClass, AActor* Causer, AActor* Target, AActor* Instigator);
};
