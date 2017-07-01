#pragma once
#include "../Effects/GAEffectExecution.h"
#include "../Effects/GAGameEffect.h"
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
