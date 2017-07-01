#pragma once
#include "GAGlobalTypes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeMod.generated.h"
/*
	Using this class you can perform actions, when change to attribute is instigated.
	Outcoming are modifiers appiled by instigator.
	Incoming are modifiers appiled by target.

	Common applications are generic mods like damage reduction by armor or other attribute.
	Increasing damage.

	Fireing other actions, when certain critera has been meet. 

	This system is actually similiar to Effects from GameEffectSystem module.

	The main difference is that, attribute mods, cannot be appiled by other objects. You can add or remove
	them on runtime, by providing set of classes, that can be added to component and the constructed.

	But you can't apply those mods by means like weapons or abilities.
	They are always passive (as they are listening for events), they are always infinite
	(their duration is for the entire lifetime of pawn/controller/game)
	and they can't affect other attribute mods. They can only affect attributes, and trigger actions
	based on them. This might include launching effects.

	As per usual using GameplayTags is recommened for defining behaviour.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ABILITYFRAMEWORK_API UGAAttributeMod : public UObject
{
	GENERATED_UCLASS_BODY()
};
