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
}
#endif //WITH_EDITOR
void UAFCueManager::HandleCue(const FGameplayTagContainer& Tags, const FGAEffectCueParams& CueParams)
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
		if (mode == ENetMode::NM_Client)
		{
			prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
		}

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
		/*	TUniquePtr<TQueue<AGAEffectCue*>>& Cues = InstancedCues.FindOrAdd(Tag);
		if (!Cues.IsValid())
		{
			Cues = MakeUnique<TQueue<AGAEffectCue*>>();
		}
		TUniquePtr<TQueue<AGAEffectCue*>>& UseCues = UsedCues.FindOrAdd(Tag);
		if (!UseCues.IsValid())
		{
			UseCues = MakeUnique<TQueue<AGAEffectCue*>>();
		}
		
		if (Cues->IsEmpty())
		{
			actor = CurrentWorld->SpawnActor<AGAEffectCue>(CueClass, Location, Rotation, SpawnParams);
			Cues->Enqueue(actor);
		}
		else
		{
			Cues->Dequeue(actor);
			UseCues->Enqueue(actor);
		}*/
		
		actor = CurrentWorld->SpawnActor<AGAEffectCue>(CueClass, Location, Rotation, SpawnParams);
		actor->NativeBeginCue(CueParams.Instigator.Get(), CueParams.HitResult.Actor.Get(),
			CueParams.Causer.Get(), CueParams.HitResult);

		/*UseCues->Dequeue(actor);
		Cues->Enqueue(actor);*/
	}
}