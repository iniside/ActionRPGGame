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
	/* Triggered when state starts. */
	virtual void BeginState(UGASAbilityState* PrevState);
	/* Triggered when state chang (old state is Ended, and new state Begins). */
	virtual void EndState();
	/* Custom. It will be triggered by UGASAbilityBase::ExecuteAbility() */
	virtual void ExecuteActionSequence();
	/* Custom. It will be triggered by UGASAbilityBase::StopAbility() */
	virtual void StopActionSequence();
	/* Custom. It will be triggered by UGASAbilityBase::FinishExecution() */
	virtual void FinishActionSequence();
};
