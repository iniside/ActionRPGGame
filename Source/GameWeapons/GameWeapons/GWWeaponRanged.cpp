// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "GWWeaponRanged.h"

AGWWeaponRanged::AGWWeaponRanged(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AGWWeaponRanged::InputPressed()
{
	BeginFire();
}
void AGWWeaponRanged::InputReleased()
{
	EndFire();
}

void AGWWeaponRanged::BeginFire()
{
	if (Role < ROLE_Authority)
	{
		ServerBeginFire();
	}
	else
	{
		OnFireBegin();
	}
}
void AGWWeaponRanged::ServerBeginFire_Implementation()
{
	BeginFire();
}
bool AGWWeaponRanged::ServerBeginFire_Validate()
{
	return true;
}

void AGWWeaponRanged::EndFire()
{
	if (Role < ROLE_Authority)
	{
		ServerEndFire();
	}
	else
	{
		OnFireEnd();
	}
}
void AGWWeaponRanged::ServerEndFire_Implementation()
{
	EndFire();
}
bool AGWWeaponRanged::ServerEndFire_Validate()
{
	return true;
}

void AGWWeaponRanged::BeginReload()
{
	if (Role < ROLE_Authority)
	{
		ServerBeginReload();
	}
	else
	{

	}
}
void AGWWeaponRanged::ServerBeginReload_Implementation()
{
	BeginReload();
}
bool AGWWeaponRanged::ServerBeginReload_Validate()
{
	return true;
}

void AGWWeaponRanged::EndReload()
{
	if (Role < ROLE_Authority)
	{
		ServerEndReload();
	}
	else
	{

	}
}
void AGWWeaponRanged::ServerEndReload_Implementation()
{
	EndReload();
}
bool AGWWeaponRanged::ServerEndReload_Validate()
{
	return true;
}

void AGWWeaponRanged::OnRep_Shooting()
{
	OnWeaponFiring();
}

void AGWWeaponRanged::OnRep_ReloadBeign()
{
	OnReloadBegin();
}

void AGWWeaponRanged::OnRep_ReloadEnd()
{
	OnReloadEnd();
}