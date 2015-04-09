#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
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
};
