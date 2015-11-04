#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAEffectTaskBase.generated.h"
/*
	Tasks are latent objects, used inside GAEffectInstanced.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffectTaskBase : public UObject
{
	GENERATED_BODY()
public:
	UGAEffectTaskBase(const FObjectInitializer& ObjectInitializer);
};
