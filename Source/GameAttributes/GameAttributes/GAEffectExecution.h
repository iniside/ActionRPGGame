#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAGameEffect.h"
#include "GAEffectGlobalTypes.h"
#include "GAEffectExecution.generated.h"

/*
	Simple base class for custom magnitude calculation.
	You can implement here any kind of formula, taking arbitrary information, for calculations.

	Object is instanced inside effect spec, so you can add custom properties set up, as per
	effect spec base. 

	Please refrain from making super complicated things, which calculate everything, it's
	generally not purpose of this class, but if you really want.. (;.
*/
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class GAMEATTRIBUTES_API UGAEffectExecution : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectExecution(const FObjectInitializer& ObjectInitializer);

	virtual void ExecuteEffect(FGAGameEffect* EffectIn, FGAEffectMod& ModIn, FGAExecutionContext& Context);
};
