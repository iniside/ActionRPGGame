// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "GAAbilityTask_WaitTargetData.h"


UGAAbilityTask_WaitTargetData::UGAAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


//UGAAbilityTask_WaitTargetData* UGAAbilityTask_WaitTargetData::WaitTargetData(UObject* WorldContextObject,
//	FName InTaskName, TSubclassOf<class AGATargetingActor> InClass, float InRange, ETraceTypeQuery InTraceChannel)
//{
//	auto MyObj = NewAbilityTask<UGAAbilityTask_WaitTargetData>(WorldContextObject, "UGAAbilityTask_WaitTargetData");
//	MyObj->Range = InRange;
//	MyObj->TraceChannel = InTraceChannel;
//	return MyObj;
//}

// ---------------------------------------------------------------------------------------
void UGAAbilityTask_WaitTargetData::Activate()
{
	UE_LOG(AbilityFramework, Log, TEXT("TArget object spawned"));
	if (Ability.IsValid())
	{
		if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
			Ability->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_WaitTargetData::OnConfirm);
		//Ability->ConfirmDelegate.CreateUObject(this, &UGAAbilityTask_WaitForConfirm::OnConfirm);
	}
}

//bool UGAAbilityTask_WaitTargetData::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class AGATargetingActor> Class, AGATargetingActor*& SpawnedActor)
//{
//	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
//	if (World)
//	{
//		SpawnedActor = World->SpawnActorDeferred<AGATargetingActor>(Class, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
//	}
//
//	if (SpawnedActor == nullptr)
//	{
//		return false;
//	}
//	UE_LOG(AbilityFramework, Log, TEXT("Begin Spawning Actor in GAAbilityTask_SpawnActor"));
//	return true;
//}
//
//void UGAAbilityTask_WaitTargetData::FinishSpawningActor(UObject* WorldContextObject, AGATargetingActor* SpawnedActor)
//{
//	if (SpawnedActor)
//	{
//		FTransform SpawnTransform;
//		SpawnedActor->FinishSpawning(SpawnTransform);
//		TargetActor = SpawnedActor;
//	}
//	ReadyForActivation();
//	UE_LOG(AbilityFramework, Log, TEXT("Finish Spawning Actor in GAAbilityTask_SpawnActor"));
//}
void UGAAbilityTask_WaitTargetData::TickTask(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction)
{
	//if (TargetActor && Ability.IsValid())
	//{
	//	FHitResult OutHit;
	//	bool bHit = Ability->LineTraceSingleByChannelFromCamera(Range, TraceChannel, false, OutHit,
	//		EDrawDebugTrace::Type::None, true, FLinearColor::Green, FLinearColor::Red, 2);
	//	if (bHit)
	//	{
	//		TargetActor->SetActorLocation(OutHit.Location);
	//	}
	//	else
	//	{
	//		TargetActor->SetActorLocation(OutHit.TraceEnd);
	//	}
	//}
}

void UGAAbilityTask_WaitTargetData::OnConfirm()
{
	Ability->OnConfirmDelegate.Clear();
	OnConfirmed.Broadcast();
	EndTask();
}