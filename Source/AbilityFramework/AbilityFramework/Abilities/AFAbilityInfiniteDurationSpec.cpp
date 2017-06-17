// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/CustomApplications/AFAttributeDurationInfinite.h"
#include "AFAbilityInfiniteDurationSpec.h"




UAFAbilityInfiniteDurationSpec::UAFAbilityInfiniteDurationSpec()
{
	Application = UAFAttributeDurationInfinite::StaticClass();
}