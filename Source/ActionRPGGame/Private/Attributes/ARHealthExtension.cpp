// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHealthExtension.h"
#include "AFAbilityComponent.h"
#include "AI/ARAICharacter.h"


void UARHealthExtension::PreAttributeModify(const FGAEffectContext& InContext, float PreValue)
{
	
}
void UARHealthExtension::PostAttributeModify(const FGAEffectContext& InContext, float PreValue, float PostValue)
{
	AARAICharacter* AIChar = Cast<AARAICharacter>(InContext.Target.Get());
	if (!AIChar)
		return;

	if (PostValue <= 0)
	{
		AIChar->Kill();
	}
}