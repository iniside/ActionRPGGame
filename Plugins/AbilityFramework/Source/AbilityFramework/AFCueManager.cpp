// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Effects/GAEffectCue.h"
#include "AFCueSet.h"
#include "AFCueManager.h"
#if WITH_EDITOR
#include "Editor.H"
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
	ManagerInstance->LoadCueSet();

	return ManagerInstance;
}
void UAFCueManager::Initialize()
{
#if WITH_EDITOR
	FEditorDelegates::EndPIE.AddUObject(this, &UAFCueManager::HandleOnPIEEnd);
#endif //WITH_EDITOR
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAFCueManager::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAFCueManager::HandlePostLoadMap);
}
void UAFCueManager::LoadCueSet()
{
	CueSet = DefaultCueSet.LoadSynchronous();
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
	const FGAEffectCueParams& CueParams)
{
	if (!CurrentWorld)
		CurrentWorld = CueParams.Instigator->GetWorld();
	for (const FGameplayTag& Tag : CueParams.CueTags)
	{
		TSubclassOf<AGAEffectCue> CueClass = CueSet->Cues.FindRef(Tag);
		if (!CueClass)
			continue;
		
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
			*Tag.ToString(),
			CueParams.Instigator.IsValid() ? *CueParams.Instigator->GetName() : TEXT("Invalid Instigator"),
			*CueParams.HitResult.Location.ToString(),
			*CurrentWorld->GetName(),
			*role
		);

		FActorSpawnParameters SpawnParams;
		FVector Location = CueParams.HitResult.Location;
		FRotator Rotation = FRotator::ZeroRotator;
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
		
		//actor = CurrentWorld->SpawnActor<AGAEffectCue>(CueClass, Location, Rotation, SpawnParams);
		if (actor)
		{
			actor->NativeBeginCue(CueParams.Instigator.Get(), CueParams.HitResult.Actor.Get(),
				CueParams.Causer.Get(), CueParams.HitResult, CueParams);
		}
		/*UseCues->Dequeue(actor);
		Cues->Enqueue(actor);*/
	}
}
void UAFCueManager::HandleRemoveCue(const FGameplayTagContainer& Tags,
	const FGAEffectCueParams& CueParams)
{
	if (!CurrentWorld)
		CurrentWorld = CueParams.Instigator->GetWorld();
	for (const FGameplayTag& Tag : CueParams.CueTags)
	{
		TSubclassOf<AGAEffectCue> CueClass = CueSet->Cues.FindRef(Tag);
		if (!CueClass)
			continue;

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
			Cues->Enqueue(actor);
			actor->NativeOnRemoved();
		}
	}
}