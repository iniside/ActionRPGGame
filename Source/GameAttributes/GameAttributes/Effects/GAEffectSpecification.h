#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.h"
#include "GAEffectSpecification.generated.h"
/*
	Class which is used as default template for effects. 

	Do not make event graph in here (I mean you can, but's never going to be executed).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffectSpecification : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectSpecification(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectName EffectName;
	/*
		Policy effect. How it stacks, where it stack, does it have period and/or duration ?
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> AttributeModifiers;

	/*
		Modifiers which will be applied on effect period.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> PeriodModifiers;
	/*
		Modifiers which will be applied when effect is prematurly removed
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> RemovedModifiers;
	/*
		Modifiers which will be applied when effect naturally expired.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> ExpiredModifiers;

	/*
		If I have duration, I will modify other effects, attribute modifiers,
		if those effects meet these requirements.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAEffectModifier> EffectModifiers;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGEffectModifierGroup> EffectModifierGroups;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectDuration EffectDuration;

	/*
		Tags, which describe this effect. Please, refrain
		from using more than one tag!
		Like Condition, Condition.Bleed, Boon, Enchatment,
		Hex, Status, whatever.
		If you use more than one tag, it might and will produce
		unpredictable results, which are going to be hard to balance out.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer EffectTags;
	/*
		I require these tags on target to be applied.
		If this is empty I will ignore it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		I will apply these tags, to target if I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer GrantedTags;
	/*
		I will add these immunity tags, to target if I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer GrantedImmunityTags;
	/*
		I require any of these tags, on other effect
		to be able to modify it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OtherEffectRequire;


	/*
		I will do something when I expire, If target have these tags.

		Ok. It's kind of dumb, I should do it differently, we need more data
		about who is target (target doesn't need to be actor, to which
		effect is applied, but for example, actor, who hit actor with this effect.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ExpiredRequireTags;

	UPROPERTY()
		FGAEffectContext Context;
};
