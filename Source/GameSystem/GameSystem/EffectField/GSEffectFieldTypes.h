#pragma once
#include "GSEffectFieldTypes.generated.h"

USTRUCT()
struct FGSEffectFieldInfo
{
	GENERATED_USTRUCT_BODY()
};


DECLARE_MULTICAST_DELEGATE(FGSOnEffectFieldStarted);
DECLARE_MULTICAST_DELEGATE(FGSOnEffectFieldEnded);