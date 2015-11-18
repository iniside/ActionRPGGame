#pragma once
#include "GASAbilityState.generated.h"
/*
	I could, probabaly remake it with interface instead of within.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbilityBase)
class GAMEABILITIES_API UGASAbilityState : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;

	virtual void Tick(float DeltaSeconds);
	virtual void BeginState(UGASAbilityState* PrevState);
	virtual void EndState();
	virtual void BeginActionSequence();
	virtual void EndActionSequence();
};
