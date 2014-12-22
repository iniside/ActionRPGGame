#pragma once
#include "GASAbilityState.generated.h"
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
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
