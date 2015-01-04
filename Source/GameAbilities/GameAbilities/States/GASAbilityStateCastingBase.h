// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStateCastingBase.generated.h"

UCLASS(DefaultToInstanced, EditInLineNew, Within = GASAbility, meta = (DisplayName = "Casting - Cast Base Dont use!"))
class UGASAbilityStateCastingBase : public UGASAbilityState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;

	virtual void ExecuteAbility();
private:
	float CurrentCastTime;
};