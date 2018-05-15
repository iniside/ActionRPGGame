// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/GAEffectCue.h"
#include "AFCueManager.h"
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
}
#if WITH_EDITOR
void UAFCueManager::HandleOnPIEEnd(bool InVal)
{
	CurrentWorld = nullptr;
	for (auto It = InstancedCues.CreateIterator(); It; ++It)
	{
		if (It->Value.IsValid())
		{
			while (!It->Value->IsEmpty())
			{
				AGAEffectCue* ToDestroy = nullptr;
				It->Value->Dequeue(ToDestroy);
				if (ToDestroy)
				{
					ToDestroy->Destroy();
				}
			}
		}
	}
	for (auto It = UsedCues.CreateIterator(); It; ++It)
	{
		if (It->Value.Num() <= 0)
		{
			UsedCues.Remove(It->Key);
		}
		for (auto QIt = It->Value.CreateIterator(); QIt; ++QIt)
		{
			if (QIt->Value.IsValid())
			{
				while (!QIt->Value->IsEmpty())
				{
					AGAEffectCue* ToDestroy = nullptr;
					QIt->Value->Dequeue(ToDestroy);
					if (ToDestroy)
					{
						ToDestroy->Destroy();
					}
				}
			}
			if (It->Value.Num() <= 0)
			{
				UsedCues.Remove(It->Key);
			}
		}

	}
	ActiveCues.Empty();
	InstancedCues.Empty();
	UsedCues.Empty();
}
#endif //WITH_EDITOR
void UAFCueManager::HandlePreLoadMap(const FString& InMapName)
{
	CurrentWorld = nullptr;

	InstancedCues.Reset();
	InstancedCues.Compact();
	UsedCues.Reset();
	UsedCues.Compact();
	//for (auto It = InstancedCues.CreateIterator(); It; ++It)
	//{
	//	if (It->Value.IsValid())
	//	{
	//		while (!It->Value->IsEmpty())
	//		{
	//			AGAEffectCue* ToDestroy = nullptr;
	//			It->Value->Dequeue(ToDestroy);
	//			if (ToDestroy)
	//			{
	//				ToDestroy->Destroy();
	//			}
	//		}
	//	}
	//}
	//for (auto It = UsedCues.CreateIterator(); It; ++It)
	//{
	//	if (It->Value.Num() <= 0)
	//	{
	//		UsedCues.Remove(It->Key);
	//	}
	//	for (auto QIt = It->Value.CreateIterator(); QIt; ++QIt)
	//	{
	//		if (QIt->Value.IsValid())
	//		{
	//			while (!QIt->Value->IsEmpty())
	//			{
	//				AGAEffectCue* ToDestroy = nullptr;
	//				QIt->Value->Dequeue(ToDestroy);
	//				if (ToDestroy)
	//				{
	//					ToDestroy->Destroy();
	//				}
	//			}
	//		}
	//		if (It->Value.Num() <= 0)
	//		{
	//			UsedCues.Remove(It->Key);
	//		}
	//	}

	//}
}
void UAFCueManager::HandlePostLoadMap(UWorld* InWorld)
{
	CurrentWorld = InWorld;
}


void UAFCueManager::HandleCue(const FGameplayTagContainer& Tags, 
	const FGAEffectCueParams& CueParams, FAFCueHandle InHandle)
{
	if (!CurrentWorld)
		CurrentWorld = CueParams.Instigator->GetWorld();
	

	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		for (const FGameplayTag& CueTag : Tags)
		{

			AGAEffectCue* actor = nullptr;
			ENetRole mode = CueParams.Instigator->GetRemoteRole();
			TUniquePtr<TQueue<AGAEffectCue*>>& Cues = InstancedCues.FindOrAdd(CueTag);
			if (Cues.IsValid())
			{
				FObjectKey InstigatorKey(CueParams.Instigator.Get());
				TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>>& UsedCuesMap = UsedCues.FindOrAdd(InstigatorKey);
				TUniquePtr<TQueue<AGAEffectCue*>>& UseCuesQueue = UsedCuesMap.FindOrAdd(CueTag);

				if (!UseCuesQueue.IsValid())
				{
					UseCuesQueue = MakeUnique<TQueue<AGAEffectCue*>>();
				}
				{
					Cues->Dequeue(actor);
					UseCuesQueue->Enqueue(actor);
				}
				if (actor)
				{
					ActiveCues.Add(InHandle, actor);
					actor->NativeBeginCue(CueParams.Instigator.Get(), CueParams.HitResult.GetActor(), CueParams.Causer.Get(), CueParams.HitResult, CueParams);
				}
				else
				{
					FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
					IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

					TArray<FAssetData> AssetData;
					FARFilter Filter;
					Filter.TagsAndValues.Add("EffectCueTagSearch", CueTag.ToString());
					AssetRegistryModule.Get().GetAssets(Filter, AssetData);
					FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("EffectCue"), AssetData[0].AssetName);
					FPrimaryAssetTypeInfo Info;
					if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
					{
						FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFCueManager::OnFinishedLoad, CueTag, PrimaryAssetId, CueParams, InHandle);

						Manager->LoadPrimaryAsset(PrimaryAssetId,
							TArray<FName>(),
							del);
					}
				}
				return;//don't try to load asset, we already have pooled instance.
			}
			

			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			TArray<FAssetData> AssetData;
			FARFilter Filter;
			Filter.TagsAndValues.Add("EffectCueTagSearch", CueTag.ToString());
			AssetRegistryModule.Get().GetAssets(Filter, AssetData);
			FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("EffectCue"), AssetData[0].AssetName);
			FPrimaryAssetTypeInfo Info;
			if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
			{
				FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFCueManager::OnFinishedLoad, CueTag, PrimaryAssetId, CueParams, InHandle);

				Manager->LoadPrimaryAsset(PrimaryAssetId,
					TArray<FName>(),
					del);
			}
		}
	}
}
void UAFCueManager::HandleExecuteCue(FAFCueHandle InHandle)
{
	AGAEffectCue** Cue = ActiveCues.Find(InHandle);
	if (Cue)
	{
		AGAEffectCue* cue = *Cue;

		cue->NativeOnExecuted();
	}
}
void UAFCueManager::OnFinishedLoad(FGameplayTag InCueTag
	, FPrimaryAssetId InPrimaryAssetId
	, FGAEffectCueParams CueParams
	, FAFCueHandle InHandle)
{
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* loaded = Manager->GetPrimaryAssetObject(InPrimaryAssetId);
		TSubclassOf<AGAEffectCue> cls = Cast<UClass>(loaded);
		if (cls)
		{
			TSubclassOf<AGAEffectCue> CueClass = cls;
			if (!CueClass)
				return;

			ENetRole mode = CueParams.Instigator->GetRemoteRole();
			FString role;
			switch (mode)
			{
			case ROLE_None:
				role = "ROLE_None";
				break;
			case ROLE_SimulatedProxy:
				role = "ROLE_SimulatedProxy";
				break;
			case ROLE_AutonomousProxy:
				role = "ROLE_AutonomousProxy";
				break;
			case ROLE_Authority:
				role = "ROLE_Authority";
				break;
			case ROLE_MAX:
				role = "ROLE_MAX";
				break;
			default:
				break;
			}

			FString prefix = "";
			//if (mode == ENetMode::NM_Client)
			//{
			//	prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
			//}

			UE_LOG(AbilityFramework, Log, TEXT("%s : CueManager HandleCue: %s, Instigator: %s, Location: %s, World: %s, Role: %s"),
				*prefix,
				*InCueTag.ToString(),
				CueParams.Instigator.IsValid() ? *CueParams.Instigator->GetName() : TEXT("Invalid Instigator"),
				*CueParams.HitResult.Location.ToString(),
				*CurrentWorld->GetName(),
				*role
			);

			FActorSpawnParameters SpawnParams;
			FVector Location = CueParams.HitResult.Location;
			FRotator Rotation = FRotator::ZeroRotator;
			AGAEffectCue* actor = nullptr;
			
			TUniquePtr<TQueue<AGAEffectCue*>>& Cues = InstancedCues.FindOrAdd(InCueTag);
			if (!Cues.IsValid())
			{
				Cues = MakeUnique<TQueue<AGAEffectCue*>>();
			}
			FObjectKey InstigatorKey(CueParams.Instigator.Get());
			TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>>& UsedCuesMap = UsedCues.FindOrAdd(InstigatorKey);
			TUniquePtr<TQueue<AGAEffectCue*>>& UseCuesQueue = UsedCuesMap.FindOrAdd(InCueTag);

			if (!UseCuesQueue.IsValid())
			{
				UseCuesQueue = MakeUnique<TQueue<AGAEffectCue*>>();
			}

			if (Cues->IsEmpty())
			{
				actor = CurrentWorld->SpawnActor<AGAEffectCue>(CueClass, Location, Rotation, SpawnParams);
				Cues->Enqueue(actor);
				Cues->Dequeue(actor);
				UseCuesQueue->Enqueue(actor);
			}
			else
			{
				Cues->Dequeue(actor);
				UseCuesQueue->Enqueue(actor);
			}

			if (actor)
			{
				ActiveCues.Add(InHandle, actor);
				actor->NativeBeginCue(CueParams.Instigator.Get(), CueParams.HitResult.Actor.Get(),
					CueParams.Causer.Get(), CueParams.HitResult, CueParams);
			}
		}

		/*{
			Manager->UnloadPrimaryAsset(InPrimaryAssetId);
		}*/
	}
}

void UAFCueManager::HandleRemoveCue(const FGameplayTagContainer& Tags,
	const FGAEffectCueParams& CueParams, FAFCueHandle InHandle)
{
	if (!CurrentWorld)
		CurrentWorld = CueParams.Instigator->GetWorld();
	for (const FGameplayTag& Tag : CueParams.CueTags)
	{
		AGAEffectCue* actor = nullptr;
		TUniquePtr<TQueue<AGAEffectCue*>>& Cues = InstancedCues.FindOrAdd(Tag);
		if (!Cues.IsValid())
		{
			Cues = MakeUnique<TQueue<AGAEffectCue*>>();
		}
		FObjectKey InstigatorKey(CueParams.Instigator.Get());
		TMap<FGameplayTag, TUniquePtr<TQueue<AGAEffectCue*>>>& UsedCuesMap = UsedCues.FindOrAdd(InstigatorKey);
		TUniquePtr<TQueue<AGAEffectCue*>>& UseCuesQueue = UsedCuesMap.FindOrAdd(Tag);

		if (!UseCuesQueue.IsValid())
		{
			UseCuesQueue = MakeUnique<TQueue<AGAEffectCue*>>();
		}

		UseCuesQueue->Dequeue(actor);
		

		if (actor)
		{
			ActiveCues.Remove(InHandle);
			Cues->Enqueue(actor);
			actor->NativeOnRemoved();
		}
	}
}