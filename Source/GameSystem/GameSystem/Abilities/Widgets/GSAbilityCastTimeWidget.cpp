// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbility.h"
#include "../GSAbilitiesComponent.h"

#include "GSAbilityCastTimeWidget.h"

UGSAbilityCastTimeWidget::UGSAbilityCastTimeWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UGSAbilityCastTimeWidget::InitializeWidget()
{

}

float const UGSAbilityCastTimeWidget::GetCurrentCastTime() const
{
	//if (AbilityComponent && AbilityComponent->ActiveAbility)
	//{
	//	return AbilityComponent->ActiveAbility->GetCurrentCastTime();
	//}
	return 0;
}

float const UGSAbilityCastTimeWidget::GetMaxCastTime() const
{
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