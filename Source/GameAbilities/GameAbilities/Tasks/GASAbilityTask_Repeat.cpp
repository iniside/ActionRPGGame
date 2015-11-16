// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilityTask_Repeat.h"




UGASAbilityTask_Repeat* UGASAbilityTask_Repeat::CreateRepeatTask(UObject* WorldContextObject)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_Repeat>(WorldContextObject);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}
