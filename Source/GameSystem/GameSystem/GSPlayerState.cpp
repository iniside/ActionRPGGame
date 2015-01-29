// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameSystem.h"
#include "Net/UnrealNetwork.h"

#include "GSPlayerState.h"

AGSPlayerState::AGSPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPlayerInCombat = false;
}
void AGSPlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGSPlayerState, bIsPlayerInCombat);
}
void AGSPlayerState::OnRep_IsPlayerInCombat()
{
	OnPlayerCombatStateChanged.Broadcast(bIsPlayerInCombat);
}