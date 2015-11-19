// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateChanneled.h"

UGASAbilityStateChanneled::UGASAbilityStateChanneled(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
}

void UGASAbilityStateChanneled::Tick(float DeltaSeconds)
{
	//GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
	CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->PeriodLenght
	//	&& GetOuterUGASAbility()->PeriodCount > CurrentPeriodCount)
	//{
	//	CurrentCastTime = 0;
	//	CurrentPeriodCount += 1;
	//	ExecuteAbility();
	//}
	//else if (CurrentPeriodCount >= GetOuterUGASAbility()->PeriodCount)
	//{
	//	GetOuterUGASAbility()->CurrentCastTime = 0;
	//	CurrentPeriodCount = 0;
	//	ChannelFinished();
	//}
}

void UGASAbilityStateChanneled::ExecuteAbility()
{
	GetOuterUGASAbilityBase()->OnAbilityExecuted();

}
void UGASAbilityStateChanneled::ExecuteChannel()
{
	GetOuterUGASAbilityBase()->OnAbilityExecuted();
}
void UGASAbilityStateChanneled::ChannelFinished()
{
	//GetOuterUGASAbility()->bIsBeingCast = false;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateChanneled::BeginState(UGASAbilityState* PrevState)
{
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());
	FTimerManager& TimerManager = GetOuterUGASAbilityBase()->GetWorld()->GetTimerManager();
	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGASAbilityStateChanneled::ExecuteChannel);
	TimerManager.SetTimer(ChannelTimerHandle, del, Interval, true, 0);
}
void UGASAbilityStateChanneled::EndState()
{

}
void UGASAbilityStateChanneled::BeginActionSequence()
{}
void UGASAbilityStateChanneled::EndActionSequence()
{
	FTimerManager& TimerManager = GetOuterUGASAbilityBase()->GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ChannelTimerHandle);
	GetOuterUGASAbilityBase()->OnNativeStopAbility();
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
}