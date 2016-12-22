// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GAAbilityTask_Repeat.h"




UGAAbilityTask_Repeat* UGAAbilityTask_Repeat::CreateRepeatTask(UObject* WorldContextObject)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_Repeat>(WorldContextObject);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}
