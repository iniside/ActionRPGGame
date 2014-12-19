#pragma once

#include "GAGlobalTypes.generated.h"

USTRUCT(BlueprintType)
struct FGAAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FName AttributeName;
};
