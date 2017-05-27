#pragma once
#include "Effects/GACustomCalculation.h"
#include "ARSpellCalculation.generated.h"

/*

*/
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPGGAME_API UARSpellCalculation : public UGACustomCalculation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGAAttributeCapture AbilityAttribute;

	UARSpellCalculation(const FObjectInitializer& ObjectInitializer);

	virtual float NativeCalculateMagnitude(const FGAEffectHandle& HandleIn) override;
};
