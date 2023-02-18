// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/AFCueActor.h"
#include "Effects/AFCueStatic.h"
#include "AFCueManager.h"
#include "GameplayTagsManager.h"

#include "Engine/ObjectLibrary.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

UAFCueManager* UAFCueManager::ManagerInstance = nullptr;
//UWorld* UAFCueManager::CurrentWorld = nullptr;

UAFCueManager* UAFCueManager::Get()
{
	if (ManagerInstance)
	{
		return ManagerInstance;
	}
	ManagerInstance = NewObject<UAFCueManager>(GEngine, UAFCueManager::StaticClass(), "UAFCueManagerInstance",
		RF_MarkAsRootSet);
	ManagerInstance->AddToRoot();
	ManagerInstance->Initialize();

	return ManagerInstance;
}
void UAFCueManager::Initialize()
{
#if WITH_EDITORONLY_DATA
	FEditorDelegates::EndPIE.AddUObject(this, &UAFCueManager::HandleOnPIEEnd);
#endif //WITH_EDITORONLY_DATA
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAFCueManager::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAFCueManager::HandlePostLoadMap);

	StaticCues = UObjectLibrary::CreateLibrary(UAFCueStatic::StaticClass(), true, GIsEditor);
	if (GIsEditor)
	{
		StaticCues->bIncludeOnlyOnDiskAssets = true;
	}

	TArray<FString> Paths;
	Paths.Add("/Game");
	StaticCues->LoadBlueprintAssetDataFromPaths(Paths, true);

	TArray<FAssetData> StaticCueAssets;
	StaticCues->GetAssetDataList(StaticCueAssets);

	FName TagProperty = GET_MEMBER_NAME_CHECKED(UAFCueStatic, EffectCueTagSearch);
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

	for (const FAssetData& CueAsset : StaticCueAssets)
	{
		const FName Tag = CueAsset.GetTagValueRef<FName>(TagProperty);

		if (!Tag.IsNone())
		{
			const FString GeneratedClassTag = CueAsset.GetTagValueRef<FString>(FBlueprintTags::GeneratedClassPath);

			FGameplayTag ReqTag = TagManager.RequestGameplayTag(Tag);
			FSoftObjectPath AssetPath;
			AssetPath.SetPath(FPackageName::ExportTextPathToObjectPath(*GeneratedClassTag));
			FAFCueData CueData;
			CueData.CueTag = ReqTag;
			CueData.AssetId = CueAsset.GetPrimaryAssetId();
			Cues.Add(CueData);
		}
	}

	for (int32 Idx = 0; Idx < Cues.Num(); Idx++)
	{
		int32& i = CuesMap.FindOrAdd(Cues[Idx].CueTag);
		i = Idx;
	}
}
#if WITH_EDITOR
void UAFCueManager::HandleOnPIEEnd(bool InVal)
{
	CurrentWorld = nullptr;
}
#endif //WITH_EDITOR
void UAFCueManager::HandlePreLoadMap(const FString& InMapName)
{
	CurrentWorld = nullptr;
}
void UAFCueManager::HandlePostLoadMap(UWorld* InWorld)
{
	CurrentWorld = InWorld;
}


void UAFCueManager::HandleCue(const FGameplayTagContainer& Tags
	, const FGAEffectCueParams& CueParams
	, EAFCueEvent CueEvent)
{
	if (!CurrentWorld)
		CurrentWorld = CueParams.Instigator->GetWorld();
	

	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		for (const FGameplayTag& CueTag : Tags)
		{
			int32 Idx = CuesMap[CueTag];
			FAFCueData Data = Cues[Idx];

			if (Data.CueClass)
			{
				HandleCueEvent(Cues[Idx].CueClass, CueParams, CueEvent);
			}
			else //load cue asynchronously
			{
				FPrimaryAssetId PrimaryAssetId = Data.AssetId;
				FPrimaryAssetTypeInfo Info;
				if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
				{
					FStreamableDelegate StreamDelegate = FStreamableDelegate::CreateUObject(this, &UAFCueManager::OnFinishedLoad, Idx, Data.AssetId, CueParams, CueEvent);
					TSharedPtr<FStreamableHandle> Handle = Manager->LoadPrimaryAsset(PrimaryAssetId, TArray<FName>(), StreamDelegate);				
				}
			}
		}
	}
}

void UAFCueManager::OnFinishedLoad(int32 Idx
	, FPrimaryAssetId InPrimaryAssetId
	, FGAEffectCueParams CueParams
	, EAFCueEvent CueEvent)
{
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* CueClass = Manager->GetPrimaryAssetObject(InPrimaryAssetId);
		size_t classSize = sizeof(*CueClass);

		Cues[Idx].CueClass = Cast<UClass>(CueClass);
		HandleCueEvent(Cues[Idx].CueClass, CueParams, CueEvent);
	}
}

void UAFCueManager::HandleRemoveCue(const FGameplayTagContainer& Tags,
	const FGAEffectCueParams& CueParams, FAFCueHandle InHandle)
{

}

void UAFCueManager::HandleCueEvent(UClass* InCueClass, const FGAEffectCueParams& InCueParams, EAFCueEvent CueEvent)
{
	if (UAFCueStatic* StaticCue = Cast<UAFCueStatic>(InCueClass->ClassDefaultObject))
	{
		switch (CueEvent)
		{
		case Activated:
			StaticCue->OnActivate(InCueParams);
			break;
		case Executed:
			StaticCue->OnExecuted(InCueParams);
			break;
		case Expire:
			StaticCue->OnExpire(InCueParams);
			break;
		case Removed:
			StaticCue->OnRemoved(InCueParams);
			break;
		default:
			break;
		}
		
	}
	else if (AAFCueActor* ActorCue = Cast<AAFCueActor>(InCueClass->ClassDefaultObject))
	{

	}
}