#pragma once
#include "GSCueTypes.generated.h"

USTRUCT()
struct FGSCueInfo
{
	GENERATED_USTRUCT_BODY()
};


DECLARE_MULTICAST_DELEGATE(FGSOnCueActionStarted);
DECLARE_MULTICAST_DELEGATE(FGSOnCueActionEnded);