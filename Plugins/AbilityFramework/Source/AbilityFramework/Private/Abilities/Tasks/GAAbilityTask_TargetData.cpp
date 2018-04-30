// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask_TargetData.h"


UGAAbilityTask_TargetData* UGAAbilityTask_TargetData::CreateTargetDataTask(UGAAbilityBase* WorldContextObject,
	FName InTaskName,
	bool bDrawDebug,
	bool bDrawCorrectedDebug,
	bool bUseCorrectedTrace,
	EGASConfirmType ConfirmTypeIn,
	float Range)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_TargetData>(WorldContextObject);

	if (MyObj)
	{
		MyObj->Range = Range;
		MyObj->ConfirmType = ConfirmTypeIn;
		MyObj->bIsTickable = false;
		MyObj->bDrawDebug = bDrawDebug;
		MyObj->bDrawCorrectedDebug = bDrawCorrectedDebug;
		MyObj->bUseCorrectedTrace = bUseCorrectedTrace;
	}
	return MyObj;
}

void UGAAbilityTask_TargetData::Activate()
{
	switch (ConfirmType)
	{
		case EGASConfirmType::Instant:
		{
			FHitResult Hit = LineTrace();
			OnReceiveTargetData.Broadcast(Hit);
			EndTask();
			break;
		}
		case EGASConfirmType::WaitForConfirm:
		{
			if (Ability.IsValid())
			{
				if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_TargetData::OnConfirm);
					bIsTickable = true;
				}
				if (!Ability->OnConfirmCastingEndedDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmCastingEndedDelegate.AddUObject(this, &UGAAbilityTask_TargetData::OnCastEndedConfirm);
				}
			}
			break;
		}
	}
}

// ---------------------------------------------------------------------------------------

void UGAAbilityTask_TargetData::OnConfirm()
{
	FHitResult Hit(ForceInit);// = LineTrace();
	OnConfirmed.Broadcast(Hit);
	Ability->OnConfirmDelegate.RemoveAll(this);
}
void UGAAbilityTask_TargetData::OnCastEndedConfirm()
{
	FHitResult Hit = LineTrace();
	OnReceiveTargetData.Broadcast(Hit);
	bIsTickable = false;
	EndTask();
}
void UGAAbilityTask_TargetData::Tick(float DeltaTime)
{
	//FHitResult HitOut = LineTrace();
}

FHitResult UGAAbilityTask_TargetData::LineTrace()
{
	FHitResult HitOut;
	APlayerController* PC = Ability->PCOwner;
	APawn* P = Ability->POwner;
	UCameraComponent* Camera = Ability->OwnerCamera;
	FVector TraceStart;
	FRotator UnusedRot;
	if (PC)
	{
		PC->PlayerCameraManager->GetCameraViewPoint(TraceStart, UnusedRot);
//		TraceStart = Camera->GetComponentLocation();
	}
	else
	{
		UnusedRot = P->GetBaseAimRotation();
		TraceStart = P->GetPawnViewLocation();
	}

	FVector TraceEnd = UnusedRot.Vector() * Range + TraceStart;
	UWorld* World = GetWorld();
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(P);
	FCollisionResponseParams ColResp;
	World->LineTraceSingleByChannel(HitOut, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, ColParams, ColResp);
	if (HitOut.bBlockingHit)
	{
		FHitResult NewHit;
		FVector Start = P->GetPawnViewLocation();
		FVector NewDir = (HitOut.Location - Start).GetSafeNormal();
		float Distance = FVector::Dist(Start, HitOut.Location);
		FVector End = Start + (NewDir * Range);
		World->LineTraceSingleByChannel(NewHit, Start, End, ECollisionChannel::ECC_WorldStatic, ColParams, ColResp);
	
		if (bDrawCorrectedDebug)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2);// GetWorld()->DeltaTimeSeconds);
			if (NewHit.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), Start, NewHit.Location, FColor::Magenta, true, 2);//GetWorld()->DeltaTimeSeconds);
				DrawDebugPoint(GetWorld(), NewHit.Location, 8, FColor::Magenta, true, 2);//GetWorld()->DeltaTimeSeconds);
			}
		}
	}
	else
	{
		FHitResult NewHit;
		FVector Start = P->GetPawnViewLocation();
		FVector NewDir = (TraceEnd - Start).GetSafeNormal();
		float Distance = Range - FVector::Dist(TraceStart, Start);
		FVector End = Start + (NewDir * Distance);

		World->LineTraceSingleByChannel(NewHit, Start, End, ECollisionChannel::ECC_WorldStatic, ColParams, ColResp);

		if (bDrawCorrectedDebug)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2);//GetWorld()->DeltaTimeSeconds);
			if (NewHit.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), Start, NewHit.Location, FColor::Magenta, true, 2);//GetWorld()->DeltaTimeSeconds);
				DrawDebugPoint(GetWorld(), NewHit.Location, 8, FColor::Magenta, true, 2);//GetWorld()->DeltaTimeSeconds);
			}
		}
	}
	if (bDrawDebug)
	{
		if (HitOut.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, HitOut.ImpactPoint, FColor::Red, true, 2);//GetWorld()->DeltaTimeSeconds);
			DrawDebugPoint(GetWorld(), HitOut.Location, 8, FColor::Red, true, 2);//GetWorld()->DeltaTimeSeconds);
		}
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, true, 2);//GetWorld()->DeltaTimeSeconds);
	}
	return HitOut;
}
