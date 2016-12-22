// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GAAbilityBase.h"

#include "GAPersistentEffectCue.h"

AGAPersistentEffectCue::AGAPersistentEffectCue(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//replicate ?
	//or just spawn independently on each client ? and then trough delegate/RepNotify tell to destroy ?
	//bReplicates = true; 
	//SetReplicates(true);
}

void AGAPersistentEffectCue::InitializeEffectCue()
{
	//if (OwningAbility)
	//{
	//	//I'm not convinced this will work as intended.
	//	//while there can be theoretically onyl one CurrentState
	//	//there is possiblty that binding all delegates at once, might make them interering with
	//	//each other.
	//	SetOwner(OwningAbility);
	//	BindCastDelegates();
	//	BindPreparationDelegates();
	//}
}

void AGAPersistentEffectCue::BindCastDelegates()
{
	//OwningAbility->OnAbilityCastStart.AddUObject(this, &AGASPersistentEffectCue::ExecuteEffect);
	//OwningAbility->OnAbilityCastEnd.AddUObject(this, &AGASPersistentEffectCue::DestroyEffectCue);
}
void AGAPersistentEffectCue::BindPreparationDelegates()
{
	//OwningAbility->OnAbilityPreparationStart.AddUObject(this, &AGASPersistentEffectCue::ExecuteEffect);
	//OwningAbility->OnAbilityPreparationEnd.AddUObject(this, &AGASPersistentEffectCue::DestroyEffectCue);
}

void AGAPersistentEffectCue::ExecuteEffect()
{
	OnEffectExecuted();
}

void AGAPersistentEffectCue::DestroyEffectCue()
{
	Destroy();
}