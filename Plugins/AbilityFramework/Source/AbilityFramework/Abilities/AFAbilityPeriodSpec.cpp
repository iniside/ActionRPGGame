// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityPeriodSpec.h"
#include "../Effects/CustomApplications/AFPeriodApplicationAdd.h"



UAFAbilityPeriodSpec::UAFAbilityPeriodSpec()
	:Super()
{
	Application = UAFPeriodApplicationAdd::StaticClass();
}