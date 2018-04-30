// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../Effects/CustomApplications/AFAtributeDurationAdd.h"
#include "AFAbilityActivationSpec.h"



UAFAbilityActivationSpec::UAFAbilityActivationSpec()
	:Super()
{
	Application = UAFAtributeDurationAdd::StaticClass();
}