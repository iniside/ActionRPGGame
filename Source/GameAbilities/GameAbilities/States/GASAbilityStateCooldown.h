// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStateCooldown.generated.h"

UCLASS(DefaultToInstanced, EditInLineNew, Within = GASAbilityBase, meta = (DisplayName = "Cooldown State"))
class UGASAbilityStateCooldown : public UGASAbilityState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;

	void FinishCooldown();
};