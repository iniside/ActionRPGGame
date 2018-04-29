// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUDPlayerInfo.h"

#include "ARPlayerController.h"
#include "Abilities/ARAbilityManagerComponent.h"

void UARHUDPlayerInfo::NativePreConstruct()
{
	Super::NativePreConstruct();
}
void UARHUDPlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		MyPC->AbilityManager->OnAbilitySetChanged.AddDynamic(this, &UARHUDPlayerInfo::OnAbilityGroupChanged);
	}
}