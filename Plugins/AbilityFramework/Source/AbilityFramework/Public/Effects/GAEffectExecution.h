#pragma once
#include "../GAGlobalTypes.h"
#include "../Attributes/GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GAEffectGlobalTypes.h"
#include "GAEffectExecution.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class ABILITYFRAMEWORK_API UGAEffectExecution : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectExecution(const FObjectInitializer& ObjectInitializer);

	virtual void PreModifyAttribute(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, const FGAEffectContext& Context);
	virtual void ExecuteEffect(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, 
		const FAFEffectParams& Params,
		const FAFFunctionModifier& Modifier = FAFFunctionModifier());
};
