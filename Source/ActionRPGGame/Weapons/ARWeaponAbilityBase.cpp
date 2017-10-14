// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponAbilityBase.h"
#include "ARWeaponAvatar.h"


void UARWeaponAbilityBase::OnAbilityInited()
{
	UWorld* world = GetWorld();
	if (!world)
		return;

	FActorSpawnParameters SpawnParams;
	AvatarActor = world->SpawnActor<AARWeaponAvatar>(WeaponAvatar, SpawnParams);
}