// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilityState.h"
#include "GASAbilityStateActive.generated.h"

UCLASS(BlueprintType, DefaultToInstanced, EditInLineNew, Within = GASAbilityBase)
class GAMEABILITIES_API UGASAbilityStateActive : public UGASAbilityState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginState(UGASAbilityState* PrevState) override;
	virtual void EndState() override;
	virtual void ExecuteActionSequence() override;
	virtual void StopActionSequence() override;
	virtual void FinishActionSequence() override;
};