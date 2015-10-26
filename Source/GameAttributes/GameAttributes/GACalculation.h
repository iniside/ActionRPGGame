#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GACalculation.generated.h"

/*
	Class which allows for implementing custom calculations for effects. Should
	always be overriden for game needs, as in game project it's going to have
	direct access to game specific attributes, which might be used for caluclations.

	It can be be overriden on per ability/effect basis or (recommended), per 
	effect/ability familiy (spells, physical, heals etc).

	When overriding Instigator function, the assumtion is that Subtraction/Divide mods
	of effect which cause damage/heal is penalty, while add/multiply is bonus.
	In Target function it's exactly opposite, 
	though it's up to user how calculations will work.

	It's not possible to query attributes/effects from outside of AttributeComponent
	and Attribute class. 
	If items/abilities/skills/etc. add bonus attributes/effects they all should
	end up at some point inside effect pool, or inside attribute.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGACalculation : public UObject
{
	GENERATED_BODY()
public:
	UGACalculation(const FObjectInitializer& ObjectInitializer);

	virtual FGAAttributeData OutgoingModifyEffect(const FGAAttributeData& DataIn);
	virtual FGAAttributeData IncomingModifyEffect(const FGAAttributeData& DataIn);

	virtual FGAGameEffect ModiifyEffect(const FGAGameEffect& EffectIn) { return EffectIn; };

	/* Modify Effect using Target attributes/modifiers. */
	virtual FGAGameEffect ModifyEffectTarget(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context) { return EffectIn; };
	/* Modify Effect using instigator attributes/modifiers. */
	virtual FGAGameEffect ModifyEffectInstigator(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context) { return EffectIn; };


	/* Modify Effect using Target attributes/modifiers. */
	virtual void ModifyEffect(FGAEffectMod& EffectIn, FGACalculationContext& Context) { };
};
