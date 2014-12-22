#pragma once
#include "GameplayTagContainer.h"
#include "GESEffectTypes.h"
#include "GESEffectTask.generated.h"
/*
	Effect task, is simple predefined object, which should perform exactly one task
	after effect is appiled.

	Possible tasks for effect:
	1. Searching (sphere sweep test, which will search for actors/object in area)
	2. Spawning actors.
	
	The difference between doing it in blueprint vs here, is that in blueprint you just
	attach these objects, to effect, set few parameters, and that's it.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEEFFECTSYSTEM_API UGESEffectTask : public UObject
{
	GENERATED_UCLASS_BODY()
};
