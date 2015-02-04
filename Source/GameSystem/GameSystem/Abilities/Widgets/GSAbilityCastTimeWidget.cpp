// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbility.h"
#include "../GSAbilitiesComponent.h"

#include "GSAbilityCastTimeWidget.h"

UGSAbilityCastTimeWidget::UGSAbilityCastTimeWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CurrentAbilityIndex = INDEX_NONE;
}

void UGSAbilityCastTimeWidget::InitializeWidget()
{
	AbilityComponent->OnGetAbilityIndex.BindUObject(this, &UGSAbilityCastTimeWidget::Del_OnAbilityChangedIndex);
}
void UGSAbilityCastTimeWidget::MarkWidgetDirty()
{
	CurrentAbilityIndex = INDEX_NONE;
}
float const UGSAbilityCastTimeWidget::GetCurrentCastTime() const
{
	if (CurrentAbilityIndex != INDEX_NONE)
	{
		return AbilityComponent->GetGASAbility(CurrentAbilityIndex)->CurrentCastTime;
	}
	//if (AbilityComponent && AbilityComponent->ActiveAbility)
	//{
	//	return AbilityComponent->ActiveAbility->GetCurrentCastTime();
	//}
	return 0;
}

float const UGSAbilityCastTimeWidget::GetMaxCastTime() const
{
	if (CurrentAbilityIndex != INDEX_NONE)
	{
		return AbilityComponent->GetGASAbility(CurrentAbilityIndex)->CastTime;
	}
	//if (AbilityComponent && AbilityComponent->ActiveAbility)
	//{
	//	return AbilityComponent->ActiveAbility->GetMaxCastTime();
	//}
	return 0;
}

void UGSAbilityCastTimeWidget::OnAbilityCastStarted()
{

}

void UGSAbilityCastTimeWidget::OnAbilityCastFinished()
{

}

void UGSAbilityCastTimeWidget::Del_OnAbilityChangedIndex(int32 IndexIn)
{
	CurrentAbilityIndex = IndexIn;
}