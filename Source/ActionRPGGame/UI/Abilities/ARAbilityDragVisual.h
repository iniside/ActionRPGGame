// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AMTypes.h"
#include "ARAbilityDragVisual.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityDragVisual : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UARAbilityManagerComponent* AbilityManager;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup Group;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMSlot AbilitySlot;
	
	
	
};
