// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "GISItemData.h"

UGISItemData::UGISItemData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bNetAddressable = false;
	bCanStack = false;
	bCanBedropped = false;
	StackCounter = 0;
}
bool UGISItemData::IsNameStableForNetworking() const
{
	return bNetAddressable;
}
void UGISItemData::SetNetAddressable()
{
	bNetAddressable = true;
}

UWorld* UGISItemData::GetWorld() const
{
	if (CurrentWorld)
		return CurrentWorld;
	return nullptr;
}

void UGISItemData::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = OwnedTags;
}

bool UGISItemData::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return OwnedTags.HasTag(TagToCheck, EGameplayTagMatchType::Explicit, EGameplayTagMatchType::Explicit);
}
bool UGISItemData::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch) const
{
	return OwnedTags.MatchesAll(TagContainer, bCountEmptyAsMatch);
}
bool UGISItemData::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch) const
{
	return OwnedTags.MatchesAny(TagContainer, bCountEmptyAsMatch);
}