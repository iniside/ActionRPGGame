#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStatePreparation.h"
#include "GASAbilityStatePreparationNoPrep.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbilityBase, meta = (DisplayName = "Prepare - No Preparation"))
class GAMEABILITIES_API UGASAbilityStatePreparationNoPrep : public UGASAbilityStatePreparation
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
