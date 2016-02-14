// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateChanneledCharged.h"

UGASAbilityStateChanneledCharged::UGASAbilityStateChanneledCharged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASAbilityStateChanneledCharged::Tick(float DeltaSeconds)
{
}

void UGASAbilityStateChanneledCharged::ExecuteAbility()
{
	//GetOuterUGASAbility()->CastAbility();
}

void UGASAbilityStateChanneledCharged::ChannelFinished()
{
}

void UGASAbilityStateChanneledCharged::BeginState(UGASAbilityState* PrevState)
{
	if (!GetOuterUGASAbilityBase()->ApplyActivationEffect())
	{
		
	}

	//if (GetOuterUGASAbilityBase()->ActivationEffect.Spec->Duration > 0
	//	&& GetOuterUGASAbilityBase()->ActivationEffect.Spec->Period > 0)
	//{
	//	UE_LOG(GameAbilities, Log, TEXT("Set activation effect in Ability: %s"), *GetName());
	//	GetOuterUGASAbilityBase()->ActivationEffectHandle = GetOuterUGASAbilityBase()->ApplyEffectToActor(
	//		GetOuterUGASAbilityBase()->ActivationEffect, GetOuterUGASAbilityBase()->ActivationEffectHandle,
	//		GetOuterUGASAbilityBase()->POwner, GetOuterUGASAbilityBase()->POwner, GetOuterUGASAbilityBase());

	//	if (GetOuterUGASAbilityBase()->ActivationEffectHandle.IsValid())
	//	{
	//		TSharedPtr<FGAGameEffect> Effect = GetOuterUGASAbilityBase()->ActivationEffectHandle.GetEffectPtr();
	//		if (!Effect->OnEffectExpired.IsBound())
	//		{
	//			UE_LOG(GameAbilities, Log, TEXT("Bind effect expiration in Ability: %s"), *GetName());
	//			Effect->OnEffectExpired.BindUObject(GetOuterUGASAbilityBase(), &UGASAbilityBase::OnActivationEffectExpired);
	//		}
	//		if (!Effect->OnEffectRemoved.IsBound())
	//		{
	//			UE_LOG(GameAbilities, Log, TEXT("Bind effect expiration in Ability: %s"), *GetName());
	//			Effect->OnEffectRemoved.BindUObject(GetOuterUGASAbilityBase(), &UGASAbilityBase::OnActivationEffectExpired);
	//		}
	//		if (!Effect->OnEffectPeriod.IsBound())
	//		{
	//			UE_LOG(GameAbilities, Log, TEXT("Bind effect period in Ability: %s"), *GetName());
	//			Effect->OnEffectPeriod.BindUObject(GetOuterUGASAbilityBase(), &UGASAbilityBase::OnActivationEffectPeriod);
	//		}
	//	}
	//}
	//else
	//{
	//	//if effect duration is 0 go to the next state.
	//	//GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
	//}
}
void UGASAbilityStateChanneledCharged::EndState()
{
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
	GetOuterUGASAbilityBase()->NativeFinishExecution();
}
void UGASAbilityStateChanneledCharged::ExecuteActionSequence()
{}
void UGASAbilityStateChanneledCharged::StopActionSequence()
{
	GetOuterUGASAbilityBase()->RemoveEffectFromActor(GetOuterUGASAbilityBase()->ActivationEffectHandle, GetOuterUGASAbilityBase()->POwner);
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
}
void UGASAbilityStateChanneledCharged::FinishActionSequence()
{}