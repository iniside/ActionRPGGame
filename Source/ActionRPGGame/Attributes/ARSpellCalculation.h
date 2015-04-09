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
};
