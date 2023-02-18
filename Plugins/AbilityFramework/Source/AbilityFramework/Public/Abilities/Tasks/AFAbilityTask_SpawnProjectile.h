// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/GAAbilityTask.h"
#include "AFAbilityTask_SpawnProjectile.generated.h"

UENUM()
enum class EAFPRojectileSpawnTraceOption : uint8
{
	DoNotTrace,
	TraceFullPath,
	OnlyTraceWhileAscending,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAFOnPRojectileSpawned);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFAbilityTask_SpawnProjectile : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FVector StartLocation;
	UPROPERTY()
		FVector EndLocation;
	UPROPERTY()
		float LaunchSpeed;
	UPROPERTY()
		float OverrideGravityZ;
	UPROPERTY()
		EAFPRojectileSpawnTraceOption TraceOption;
	UPROPERTY()
		float CollisionRadius;
	UPROPERTY()
		bool bFavorHighArc;
	UPROPERTY()
		bool bDrawDebug;

	UPROPERTY(BlueprintAssignable)
		FAFOnPRojectileSpawned Played;
public:
	static UAFAbilityTask_SpawnProjectile* Ability_SpawnProjectile(UGAAbilityBase* WorldContextObject,
		FName InTaskName, 
		FVector InStartLocation, 
		FVector InEndLocation, 
		float InLaunchSpeed, 
		float InOverrideGravityZ, 
		EAFPRojectileSpawnTraceOption InTraceOption,
		float InCollisionRadius, 
		bool InbFavorHighArc,
		bool InbDrawDebug);
	
};
