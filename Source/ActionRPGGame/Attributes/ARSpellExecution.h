#pragma once
#include "GAEffectExecution.h"
#include "GAGameEffect.h"
#include "ARSpellExecution.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARSpellExecution : public UGAEffectExecution
{
	GENERATED_BODY()
public:
	UARSpellExecution(const FObjectInitializer& ObjectInitializer);

	virtual void ExecuteEffect(FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAExecutionContext& Context) override;
};
