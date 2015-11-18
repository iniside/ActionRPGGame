// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStateCastingBase.h"
#include "GASAbilityStateCastingCharged.generated.h"

UCLASS(DefaultToInstanced, EditInLineNew, Within = GASAbilityBase, meta = (DisplayName = "Casting - Casting Charged"))
class UGASAbilityStateCastingCharged : public UGASAbilityStateCastingBase
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;

	virtual void ExecuteAbility() override;
private:
	float CurrentCastTime;
};