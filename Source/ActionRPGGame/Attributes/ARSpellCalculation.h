#pragma once
#include "GACalculation.h"
#include "ARSpellCalculation.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARSpellCalculation : public UGACalculation
{
	GENERATED_BODY()
public:
	UARSpellCalculation(const FObjectInitializer& ObjectInitializer);

	virtual FGAAttributeData OutgoingModifyEffect(const FGAAttributeData& DataIn) override;
	virtual FGAAttributeData IncomingModifyEffect(const FGAAttributeData& DataIn) override;

	virtual FGAGameEffect ModiifyEffect(const FGAGameEffect& EffectIn) override;

	/* Modify Effect using Target attributes/modifiers. */
	virtual FGAGameEffect ModifyEffectTarget(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context) override;
	/* Modify Effect using instigator attributes/modifiers. */
	virtual FGAGameEffect ModifyEffectInstigator(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context) override;
	
	virtual void ModifyEffect(FGAEffectMod& EffectIn, FGACalculationContext& Context) override;
};
