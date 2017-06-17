// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"

#include "Effects/CustomApplications/AFPeriodApplicationInfiniteAdd.h"

#include "AFAbilityInfinitePeriodicSpec.h"




UAFAbilityInfinitePeriodicSpec::UAFAbilityInfinitePeriodicSpec()
{
	Application = UAFPeriodApplicationInfiniteAdd::StaticClass();
}