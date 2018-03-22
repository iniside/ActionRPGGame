// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityCrosshairInfo.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "Abilities/GAAbilityBase.h"
#include "ARPlayerController.h"
#include "Abilities/ARAbilityBase.h"
#include "Abilities/ARAbilityManagerComponent.h"

void UARAbilityCrosshairInfo::NativePreConstruct()
{
	Super::NativePreConstruct();

	CrosshairImage->SetBrushFromMaterial(CrosshairMaterial);
}
void UARAbilityCrosshairInfo::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairImage->SetBrushFromMaterial(CrosshairMaterial);
}

void UARAbilityCrosshairInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float CooldownRemaining = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(1, 0), GetRemainingCooldown());

	CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Percentage"), CooldownRemaining);
}