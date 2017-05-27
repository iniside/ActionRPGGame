#pragma once
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"
#include "ARSpellExecution.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARSpellExecution : public UGAEffectExecution
{
	GENERATED_BODY()
public:
	UARSpellExecution(const FObjectInitializer& ObjectInitializer);
};
