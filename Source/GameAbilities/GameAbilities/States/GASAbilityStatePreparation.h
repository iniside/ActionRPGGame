#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStatePreparation.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbilityBase, meta = (DisplayName = "Prepare - Prepare base - Dont Use!"))
class GAMEABILITIES_API UGASAbilityStatePreparation : public UGASAbilityState
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;
};
