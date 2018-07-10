// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"

#include "Engine/AssetManager.h"

#include "AFCueStatic.h"


UAFCueStatic::UAFCueStatic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAFCueStatic::PostInitProperties()
{
	UpdateAssetRegistryInfo();
	Super::PostInitProperties();
}

void UAFCueStatic::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		UpdateAssetRegistryInfo();
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		UpdateAssetRegistryInfo();
	}
}
#if WITH_EDITORONLY_DATA
void UAFCueStatic::UpdateAssetBundleData()
{
	AssetBundleData.Reset();
	UpdateAssetRegistryInfo();

	// By default parse the metadata
	if (UAssetManager::IsValid())
	{
		UAssetManager::Get().InitializeAssetBundlesFromMetadata(this, AssetBundleData);
	}
}

void UAFCueStatic::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	UpdateAssetBundleData();

	if (UAssetManager::IsValid())
	{
		// Bundles may have changed, refresh
		UAssetManager::Get().RefreshAssetData(this);
	}
}
#endif
void UAFCueStatic::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	FAssetBundleData OldData = AssetBundleData;

	UpdateAssetBundleData();

	if (UAssetManager::IsValid() && OldData != AssetBundleData)
	{
		// Bundles changed, refresh
		UAssetManager::Get().RefreshAssetData(this);
	}
#endif
}
FPrimaryAssetId UAFCueStatic::GetPrimaryAssetId() const
{
	FName dupa1 = FPackageName::GetShortFName(GetOutermost()->GetFName());

	const UAFCueStatic* A = this;
	return FPrimaryAssetId(FPrimaryAssetType("StaticCue"), dupa1);
	//if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UClass* SearchNativeClass = GetClass();

		while (SearchNativeClass && !SearchNativeClass->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic))
		{
			SearchNativeClass = SearchNativeClass->GetSuperClass();
		}

		if (SearchNativeClass && SearchNativeClass != GetClass())
		{
			// If blueprint, return native class and asset name

		}

		// Native CDO, return nothing
		return FPrimaryAssetId();
	}
}
void  UAFCueStatic::UpdateAssetRegistryInfo()
{
	EffectCueTagSearch = CueTag.GetTagName();
}

bool UAFCueStatic::OnExecuted_Implementation(const FGAEffectCueParams& Hit) const
{
	return true;
}
void UAFCueStatic::OnActivate_Implementation(const FGAEffectCueParams& Hit) const
{
}
void UAFCueStatic::OnExpire_Implementation(const FGAEffectCueParams& Hit) const
{
}
void UAFCueStatic::OnRemoved_Implementation(const FGAEffectCueParams& Hit) const
{
}

UWorld* UAFCueStatic::GetWorld() const
{
	/*TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();
	UWorld* World = nullptr;
	for (const FWorldContext& Ctx : WorldContexts)
	{
		World = Ctx.World();
	}*/
	return nullptr;
}