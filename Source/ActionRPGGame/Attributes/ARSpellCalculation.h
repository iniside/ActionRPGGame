#pragma once
#include "GACustomCalculation.h"
#include "ARSpellCalculation.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARSpellCalculation : public UGACustomCalculation
{
	GENERATED_BODY()
public:
	UARSpellCalculation(const FObjectInitializer& ObjectInitializer);
};
