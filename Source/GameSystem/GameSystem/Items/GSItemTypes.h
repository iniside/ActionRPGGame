#pragma once
#include "GAGlobalTypes.h"
#include "GSItemTypes.generated.h"

USTRUCT(BlueprintType)
struct FGSItemAttributes
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float Value;
};