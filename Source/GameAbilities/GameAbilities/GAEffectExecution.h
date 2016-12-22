#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GAEffectGlobalTypes.h"
#include "GAEffectExecution.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class GAMEABILITIES_API UGAEffectExecution : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectExecution(const FObjectInitializer& ObjectInitializer);

	virtual void PreModifyAttribute(FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAEffectContext& Context);
	void ExecuteEffect(FGAEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAEffectContext& Context);
};
