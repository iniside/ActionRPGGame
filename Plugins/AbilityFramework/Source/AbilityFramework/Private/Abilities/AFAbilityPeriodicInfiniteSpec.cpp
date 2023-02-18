// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/CustomApplications/AFPeriodApplicationInfiniteAdd.h"
#include "AFAbilityPeriodicInfiniteSpec.h"




UAFAbilityPeriodicInfiniteSpec::UAFAbilityPeriodicInfiniteSpec()
	:Super()
{
	Application = UAFPeriodApplicationInfiniteAdd::StaticClass();
}
