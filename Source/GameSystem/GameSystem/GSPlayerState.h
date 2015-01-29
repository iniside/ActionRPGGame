// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GSPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGSOnPlayerCombatStateChanged, bool);
UCLASS()
class GAMESYSTEM_API AGSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AGSPlayerState(const FObjectInitializer& ObjectInitializer);
	/*
		Generic delegate indicating if player is in combat state or not. Broadcasted, when
		bIsPlayerInCombat is replicated.
	*/
	FGSOnPlayerCombatStateChanged OnPlayerCombatStateChanged;

	UPROPERTY(ReplicatedUsing=OnRep_IsPlayerInCombat)
		bool bIsPlayerInCombat;
	UFUNCTION()
		void OnRep_IsPlayerInCombat();
};



