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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectDuration EffectDuration;

	/*
	Context of effect containing info about target, instigator, causer etc.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Base")
		FGAEffectContext Context;
	/*
	Spec Containing attributes, which this effect will modify.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeEffectSpec AttributeSpec;

	/*
	Modifiers, which are applied to other effects.
	They are not applied directly or automatically, if you want them to applied
	you will need to override, UGACalculation class.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAEffectModifierSpec> EffectModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAConditonalEffectSpec> ConditionalEffects;
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
		All of these Tags, must be present in effect target AffectTags, if the target effect is going
		to be modified, byt EffectModifiers.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
	Select calculation type, which will be used, for calculating final value of
	this effect.
	*/
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UGACalculation> CalculationType;
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
