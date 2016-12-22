// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GASAbilityTargetingObject.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTargetingObject : public UObject
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class UGAAbilityBase> AbilityOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Config")
		bool bDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Config")
		float Range;
	
	virtual FHitResult GetTarget();

	virtual class UWorld* GetWorld() const;
};
