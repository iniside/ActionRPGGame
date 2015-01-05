// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStateCastingBase.h"
#include "GASAbilityStateChanneledLocked.generated.h"

/**
 *	Channeled state. Once you press input, state will execute X number of periods, every X
 *	amount of time. Locked mean, that trace is done only once, and all subsequent periods, are
 *	targeted at initial hit location.
 */
UCLASS(DefaultToInstanced, EditInLineNew, Within = GASAbility, meta = (DisplayName = "Casting - Channeled Locked"))
class UGASAbilityStateChanneledLocked : public UGASAbilityStateCastingBase
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;

	virtual void ExecuteAbility() override;

	void ChannelFinished();
	void ChannelStarted();
private:
	float CurrentCastTime;
	float CurrentPeriodCount;
};