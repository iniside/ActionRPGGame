// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GAAbilityBase.h"
#include "ARAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityBase : public UGAAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "UI Config")
		class UARAbilityUIData* UIData;
	
	
};
