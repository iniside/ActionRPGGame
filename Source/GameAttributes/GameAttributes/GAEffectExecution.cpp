// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffectExecution.h"

UGAEffectExecution::UGAEffectExecution(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGAEffectExecution::ExecuteEffect(FGAGameEffect* EffectIn, FGAEffectMod& ModIn, FGAExecutionContext& Context)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution class implementation"));
	
	FGAAttributeBase* TargetAttribute = Context.GetTargetAttribute(ModIn.Attribute);
	if (TargetAttribute)
	{
		switch (ModIn.ChangeType)
		{
		case EGAAttributeChangeType::Damage:
			UE_LOG(GameAttributesEffects, Log, TEXT("Sample execution::Damage Value = %f"), ModIn.Value);
			TargetAttribute->Subtract(ModIn.Value);
			break;
		case EGAAttributeChangeType::DamagePercentage:
			break;
		case EGAAttributeChangeType::Heal:
			TargetAttribute->Add(ModIn.Value);
			break;
		case EGAAttributeChangeType::HealPercentage:
			break;
		}
	}
}