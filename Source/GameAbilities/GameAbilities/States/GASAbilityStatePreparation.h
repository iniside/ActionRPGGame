#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStatePreparation.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASAbilityStatePreparation : public UGASAbilityState
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
