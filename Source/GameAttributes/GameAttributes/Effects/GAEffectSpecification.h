#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAAttributes.h"
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
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> AttributeModifiers;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectDuration EffectDuration;

	/*
		My tag. For example Hex, Enchatment, Condition.Weakness, Condition.Bleed, Condition.Burning etc.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTag MyTag;
	/*
		I will apply these immunit tags, to target.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ImmunityTags;
	/*
		These tags must be present on actor, if I'm to take any effect.

		If empty I will be applied regardless of tags owned by target.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		I will add these tags, to target, when I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ApplyTags;
};
