#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "Effects/GAEffect.h"
#include "GSEffect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSEffect : public UGAEffect
{
	GENERATED_BODY()
public:
	UGSEffect(const FObjectInitializer& ObjectInitializer);


};
