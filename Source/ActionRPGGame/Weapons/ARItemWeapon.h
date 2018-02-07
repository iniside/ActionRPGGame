// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "ARItemWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ACTIONRPGGAME_API UARItemWeapon : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Ability")
		FGameplayTag Ability;
	UPROPERTY(EditAnywhere, Category = "Visual")
		TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	
	
};
