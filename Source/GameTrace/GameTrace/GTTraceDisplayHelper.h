#pragma once
#include "GameTrace.h"

#include "GTTraceDisplayHelper.generated.h"
/*
	Base class for targeting helerps, like targeting circle, spheres, splines etc.
	Displayed for particular trace action.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMETRACE_API AGTTraceDisplayHelper : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Trace")
	class UGTTraceBase* TraceAction;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPostInitialize();
};
