#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.h"
#include "GAEffectSpecification.generated.h"
/*
	Class which is used as default template for effects. 

	Effect Specifiction is not effect per se, it just contain data, from which various
	types of effects might be constructed.
	
	We have few effect types, which works independently of each other and can be made from single
	spec:
	1. Attribute Modifiers - from them effect modifing attributes will be created.
	These are most complicated effects, as it can apply conditional effects, use custom calculations,
	use instance and non-instanced effect extensions.
	2. Effect Modifiers - these will create effects, which will modify Attribute Modifiers,
	on other effects. You can think of this part as ordinary buff/debuff

	Do not make event graph in here (I mean you can, but's never going to be executed).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffectSpecification : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectSpecification(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectName EffectName;
	/*
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectDuration EffectDuration;

	/*
		Spec Containing attributes, which this effect will modify.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeEffectSpec AttributeSpec;
	/*
		Select calculation type, which will be used, for calculating final value of
		this effect attributes.
	*/
	UPROPERTY(EditAnywhere, Category = "Attributes")
		TSubclassOf<class UGACalculation> CalculationType;
	/*
		Modifier which is applied for effect duration.
		It's removed when effect is removed/expire.

		This is only modifier, which is reversed after effect is removed/expired.

		Duration effects should only be used to modify Complex Attributes!
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> AttributeModifier;
	/*
		Modifiers, which are applied to other effects.
		They are not applied directly or automatically, if you want them to applied
		you will need to override, UGACalculation class.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Modifiers")
		TArray<FGAEffectModifierSpec> EffectModifiers;
	/*
		These tags, must be present in other effect AffectTags, otherwise
		EffectModifiers won't be applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Effect Modifiers")
		FGameplayTagContainer RequiredTags;
	/*
		These effects are applied along this effect, when tag requirments for
		effect are meet.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditonals")
		TArray<FGAConditonalEffectSpec> ConditionalApplicationEffects;

	/*
		These effects, are triggered, when other effects is being applied to the same
		target as mine, and if the tags on incoming effect match, I will
		apply these effects.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditionals")
		TArray<FGAConditonalEffectSpec> ConditionalTriggerEffects;

	/*
		Tag, which are owned by me, and best describe me.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer MyTags;
	/*
		I can be affected by modifiers with these tags.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer AffectTags;
	/*
		If target have any of these tags, I will not be appilied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer IgnoreTags;
	/*
		I will apply these tags, when I'm succefully applied to target.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer AppliedTags;
	/*
		If you don't want  effect to spawn Cue, you can simply leave it empty.
	*/
	UPROPERTY(EditAnywhere, Category = "Cues")
		TSubclassOf<class AGAEffectCue> EffectCue;

	UPROPERTY(EditAnywhere, Category = "Cues")
		TSubclassOf<class UGAUIData> UIData;



	TArray<FGAAttributeData> GetInitialAttribute(const FGAEffectContext& ContextIn);
	TArray<FGAAttributeData> GetDurationAttribute(const FGAEffectContext& ContextIn);
	TArray<FGAAttributeData> GetPeriodAttribute(const FGAEffectContext& ContextIn);
	TArray<FGAAttributeData> GetRemovedAttribute(const FGAEffectContext& ContextIn);
	TArray<FGAAttributeData> GetExpiredAttribute(const FGAEffectContext& ContextIn);

	TArray<FGAEffectModifierSpec> GetEffectModifiers(const FGAEffectContext& ContextIn);
};
