#pragma once
#include "GTGlobalTypes.generated.h"

USTRUCT(BlueprintType)
struct GAMETRACE_API FemptyStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;
};

//USTRUCT(BlueprintType)
//struct GAMETRACE_API FSocketName
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FName SocketName;
//};
