#pragma once
#include "../GAEffectExecution.h"
#include "../GAGameEffect.h"
#include "GASpellExecutionTest.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class UGASpellExecutionTest : public UGAEffectExecution
{
	GENERATED_BODY()
public:
	UGASpellExecutionTest(const FObjectInitializer& ObjectInitializer);
};
