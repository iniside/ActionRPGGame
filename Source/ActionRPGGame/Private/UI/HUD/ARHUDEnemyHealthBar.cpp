// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUDEnemyHealthBar.h"
#include "Components/ProgressBar.h"

void UARHUDEnemyHealthBar::UpdateHealth(float NormalizedHealth)
{
	HealthBar->SetPercent(NormalizedHealth);
}