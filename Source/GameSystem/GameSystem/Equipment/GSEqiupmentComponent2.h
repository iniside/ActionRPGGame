#pragma once
#include "GSEqiupmentComponent2.generated.h"
/*
	Base class containing informations about items. Like trash items, 
	equipment, etc.

	1. Contain item attributes (possibly ?).
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSEqiupmentComponent2 : public UActorComponent
{
	GENERATED_BODY()
public:
	UGSEqiupmentComponent2(const FObjectInitializer& ObjectInitializer);
};
