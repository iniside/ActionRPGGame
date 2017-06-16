// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/CustomApplications/AFAtributeDurationAdd.h"
#include "AFAbilityCooldownSpec.h"




UAFAbilityCooldownSpec::UAFAbilityCooldownSpec()
	: Super()
{
	Application = UAFAtributeDurationAdd::StaticClass();
}