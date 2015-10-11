#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GACalculation.generated.h"

/*

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
	virtual FGAGameEffect ModifyEffectTarget(const FGAGameEffect& EffectIn) { return EffectIn; };
	/* Modify Effect using instigator attributes/modifiers. */
	virtual FGAGameEffect ModifyEffectInstigator(const FGAGameEffect& EffectIn) { return EffectIn; };
};
