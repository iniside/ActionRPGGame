// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityTask_SpawnProjectile.h"




UAFAbilityTask_SpawnProjectile* UAFAbilityTask_SpawnProjectile::Ability_SpawnProjectile(UGAAbilityBase* WorldContextObject,
	FName InTaskName,
	FVector InStartLocation,
	FVector InEndLocation,
	float InLaunchSpeed,
	float InOverrideGravityZ,
	EAFPRojectileSpawnTraceOption InTraceOption,
	float InCollisionRadius,
	bool InbFavorHighArc,
	bool InbDrawDebug)
{
	auto MyObj = NewAbilityTask<UAFAbilityTask_SpawnProjectile>(WorldContextObject, InTaskName);
	MyObj->StartLocation = InStartLocation;
	MyObj->EndLocation = InEndLocation;
	MyObj->LaunchSpeed = InLaunchSpeed;
	MyObj->OverrideGravityZ = InOverrideGravityZ;
	MyObj->TraceOption = InTraceOption;
	MyObj->CollisionRadius = InCollisionRadius;
	MyObj->bFavorHighArc = InbFavorHighArc;
	MyObj->bDrawDebug = InbDrawDebug;
	return MyObj;
}