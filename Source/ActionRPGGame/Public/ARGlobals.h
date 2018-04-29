// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "GameplayTags.h"

/**
 * 
 */
class ACTIONRPGGAME_API FARGlobals
{
public:
	FARGlobals();
	~FARGlobals();
	static FPrimaryAssetId MakeAbilityAssetId(const FAssetData& InAssetData)
	{
		return FPrimaryAssetId(FPrimaryAssetType("Ability"), InAssetData.AssetName);
	}
	static FAssetData GetAbilityAssetByTag(const FGameplayTag& InTag)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AssetData;
		FARFilter Filter;
		Filter.TagsAndValues.Add("AbilityTagSearch", InTag.ToString());
		AssetRegistryModule.Get().GetAssets(Filter, AssetData);
		if (AssetData.Num() == 1)
			return AssetData[0];

		return FAssetData();
	}
};
