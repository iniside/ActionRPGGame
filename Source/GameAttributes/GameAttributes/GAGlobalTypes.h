#pragma once

#include "GAGlobalTypes.generated.h"

UENUM()
enum class EGAAttributeOp : uint8
{
	Add,
	Subtract,
	Multiply,
	Divide,
	Set,

	Invalid
};

USTRUCT(BlueprintType)
struct FGAAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FName AttributeName;
};
