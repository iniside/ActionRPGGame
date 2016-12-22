#pragma once
#include "GASAbilityMod.generated.h"
/*
	AbilityMod is base class for ability modifications. 
	It can change or define things like ability resource cost, timers (cooldown, casting time).

	It's mainly done to make system like runtime modification of abilities easier. When player 
	want to modify ability by draggin on it some upgrade (subclass of this base class), which
	will then be spawned initialized and start listening for ability action, or just plain
	modify properties while storing the original ones as well.

	One Mod should modify one property of owning ability. Though I will try to add
	support for multiple modifications per single mod.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GAAbilityBase)
class GAMEABILITIES_API UGASAbilityMod : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
};
