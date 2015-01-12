// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GSAbility.h"

#include "Net/UnrealNetwork.h"

#include "GSAbilityInfo.h"

UGSAbilityInfo::UGSAbilityInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UGSAbilityInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGSAbilityInfo, AbilityType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGSAbilityInfo, ActiveAbility, COND_OwnerOnly);
}

UTexture2D* UGSAbilityInfo::GetImage()
{
	if (AbilityType && AbilityType.GetDefaultObject())
	{
		return AbilityType.GetDefaultObject()->AbilityIcon;
	}
	return nullptr;
}

bool UGSAbilityInfo::OnItemAddedToSlot()
{
	if (AbilityType)
	{
		ActiveAbility = GetWorld()->SpawnActor<AGSAbility>(AbilityType);
		ActiveAbility->SetOwner(CurrentOwner);
		ActiveAbility->SetPawnOwner(OwningPawn);
		ActiveAbility->SetPlayerController(OwiningPlayerController);
	}
	return false;
}
bool UGSAbilityInfo::OnItemRemovedFromSlot()
{
	return false;
}

bool UGSAbilityInfo::InputPressed_Implementation()
{
	if (ActiveAbility)
	{
		ActiveAbility->InputPressed();
		return true;
	}
	return false;
}
bool UGSAbilityInfo::InputReleased_Implementation()
{
	if (ActiveAbility)
	{
		ActiveAbility->InputReleased();
		return true;
	}
	return false;
}

bool UGSAbilityInfo::CanItemBeSwapped()
{
	if (ActiveAbility)
	{
		if (ActiveAbility->GetIsBeingCast() || ActiveAbility->GetIsOnCooldown())
		{
			return false;
		}
	}
	return true;
}