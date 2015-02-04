// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../Items/GSItemWeaponInfo.h"

#include "GSAbility.h"

UGSAbility::UGSAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	OnSetWeaponsForAbility.AddUObject(this, &UGSAbility::SetWeaponForAbility);

	OnSetWeaponsForAbility.AddUObject(this, &UGSAbility::SetOnSetWeaponsforability);
}

//bool UGSAbility::CheckIfCanUseAbility()
//{
//	return true; //implement checking for attribute.
//}

void UGSAbility::SubtractAttributeCost()
{

}
//void AGSAbility::OnRep_CastStarted()
//{
//	Super::OnRep_CastStarted();
//	OnCueActionStarted.Broadcast();
//}
//void AGSAbility::OnRep_CastEnded()
//{
//	Super::OnRep_CastEnded();
//	OnCueActionEnded.Broadcast();
//}

void UGSAbility::SetWeaponForAbility(class UGSItemWeaponInfo* LeftWeaponIn, class UGSItemWeaponInfo* RightWeaponIn)
{
	CurrentLeftWeapon = LeftWeaponIn;
	CurrentRightWeapon = RightWeaponIn;
}

//FVector UGSAbility::GetSocketLocation(FName SocketNameIn)
//{
//	return Super::GetSocketLocation(SocketNameIn);
//}
//
//FVector UGSAbility::GetTraceStartLocation()
//{
//	if (CurrentLeftWeapon)
//	{
//		return CurrentLeftWeapon->GetCrosshairTraceStartLocation();
//	}
//
//	if (CurrentRightWeapon)
//	{
//		return CurrentRightWeapon->GetCrosshairTraceStartLocation();
//	}
//	return GetSocketLocation(TargetTraceSocket);
//}

void UGSAbility::SetOnSetWeaponsforability(class UGSItemWeaponInfo* LeftWeaponIn, class UGSItemWeaponInfo* RightWeaponIn)
{
	CurrentLeftWeapon = LeftWeaponIn;
	CurrentRightWeapon = RightWeaponIn;
}