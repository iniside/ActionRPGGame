// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "Abilities/GAAbilityBase.h"
#include "GAAbilityTask_TargetDataLineTrace.h"


UGAAbilityTask_TargetDataLineTrace* UGAAbilityTask_TargetDataLineTrace::CreateTargetDataLineTrace(UGAAbilityBase* WorldContextObject
	, FName InTaskName
	, ETraceTypeQuery InTraceChannel
	, USkeletalMeshComponent* InSocketComponent
	, FName InSocketName
	, bool bDrawDebug
	, EAFConfirmType ConfirmTypeIn
	, float Range)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_TargetDataLineTrace>(WorldContextObject, InTaskName);

	if (MyObj)
	{
		MyObj->TraceChannel = InTraceChannel;
		MyObj->Range = Range;
		MyObj->ConfirmType = ConfirmTypeIn;
		MyObj->SocketComponent = InSocketComponent;
		MyObj->SocketName = InSocketName;
		MyObj->bIsTickable = false;
		MyObj->bDrawDebug = bDrawDebug;
	}
	return MyObj;
}
UGAAbilityTask_TargetDataLineTrace::UGAAbilityTask_TargetDataLineTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsReplicated = true;
}
void UGAAbilityTask_TargetDataLineTrace::Activate()
{
	LocalHitResult.Reset(1, false);
	FHitResult HitData;
	switch (ConfirmType)
	{
		case EAFConfirmType::Instant:
		{
			
			HitData = LineTrace();
			LocalHitResult = HitData;
			//OnClient... Is called on both Client and server and is result of local simulation
			//unconfirmed by server. This result might get overrided when data from server arrive to client
			//it's good to spawn some cosmetic effects, but shouldn't be used to actually confirm hit result on client.

			//OnServer.. is confirmed by server that we got hit (or not), and should be used to show client confirmation
			//for hits.
			if (IsServerOrStandalone())
			{
				//OnClientReceiveTargetData.Broadcast(HitData);
				OnServerReceiveTargetData.Broadcast(HitData);
				
			}
			else
			{
				OnClientUnconfirmedTargetData.Broadcast(HitData);
			}
			break;
		}
		case EAFConfirmType::WaitForConfirm:
		{
			if (Ability.IsValid())
			{
				if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_TargetDataLineTrace::OnConfirm);
					bIsTickable = true;
				}
				if (!Ability->OnConfirmCastingEndedDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmCastingEndedDelegate.AddUObject(this, &UGAAbilityTask_TargetDataLineTrace::OnCastEndedConfirm);
				}
			}
			break;
		}
	}
	//if (HitData.IsValidBlockingHit())
	{
		if (AbilityComponent->GetOwnerRole() < ROLE_Authority)
		{
			APlayerController* PC = Ability->PCOwner;
			APawn* P = Ability->POwner;

			PC->PlayerState->RecalculateAvgPing();
			float ExactPing = PC->PlayerState->ExactPing;

			FAFLineTraceData TraceData;
			TraceData.ExactPing = ExactPing;
			TraceData.HitActor = HitData.GetActor();
			TraceData.HitLocation = HitData.Location;
			ServerConfirmHitInfo(TraceData);
			
		}
	}
	if (AbilityComponent->GetOwnerRole() == ROLE_Authority)
	{
		EndTask();
	}
	
}

void UGAAbilityTask_TargetDataLineTrace::ServerConfirmHitInfo_Implementation(FAFLineTraceData TraceData)
{
	int test = 0;
	UE_LOG(AbilityFramework, Log, TEXT("%s Server Confirm Hit"), *GetName());
	FAFLineTraceConfirmData ConfirmData;
	ConfirmData.bConfirmed = true;
	ConfirmData.HitActor = LocalHitResult.GetActor();
	ConfirmData.HitLocation = LocalHitResult.Location;
	ClientConfirmHitInfo(ConfirmData);
}
bool UGAAbilityTask_TargetDataLineTrace::ServerConfirmHitInfo_Validate(FAFLineTraceData TraceData)
{
	return true;
}

void UGAAbilityTask_TargetDataLineTrace::ClientConfirmHitInfo_Implementation(FAFLineTraceConfirmData ConfirmData)
{
	if (ConfirmData.bConfirmed)
	{
		UE_LOG(AbilityFramework, Log, TEXT("%s Client Hit Confirmed"), *GetName());
		OnClientReceiveTargetData.Broadcast(LocalHitResult);
	}
	else
	{
		UE_LOG(AbilityFramework, Log, TEXT("%s Client Hit Overrided"), *GetName());
		LocalHitResult.Actor = ConfirmData.HitActor;
		LocalHitResult.Location = ConfirmData.HitLocation;
		LocalHitResult.ImpactPoint = ConfirmData.HitLocation;
		OnClientReceiveTargetData.Broadcast(LocalHitResult);
	}
	EndTask();
}

// ---------------------------------------------------------------------------------------

void UGAAbilityTask_TargetDataLineTrace::OnConfirm()
{
	FHitResult Hit = LineTrace();
	//OnConfirmed.Broadcast(Hit);

	Ability->OnConfirmDelegate.RemoveAll(this);
}
void UGAAbilityTask_TargetDataLineTrace::OnCastEndedConfirm()
{
	FHitResult Hit = LineTrace();
	LocalHitResult = Hit;
	//OnClient... Is called on both Client and server and is result of local simulation
	//unconfirmed by server. This result might get overrided when data from server arrive to client
	//it's good to spawn some cosmetic effects, but shouldn't be used to actually confirm hit result on client.

	//OnServer.. is confirmed by server that we got hit (or not), and should be used to show client confirmation
	//for hits.
	if (AbilityComponent->GetOwnerRole() < ROLE_Authority)
	{
		APlayerController* PC = Ability->PCOwner;
		APawn* P = Ability->POwner;

		PC->PlayerState->RecalculateAvgPing();
		float ExactPing = PC->PlayerState->ExactPing;

		FAFLineTraceData TraceData;
		TraceData.ExactPing = ExactPing;
		TraceData.HitActor = Hit.GetActor();
		TraceData.HitLocation = Hit.Location;
		ServerConfirmHitInfo(TraceData);

	}

	if (IsServerOrStandalone())
	{
		//OnClientReceiveTargetData.Broadcast(HitData);
		OnServerReceiveTargetData.Broadcast(Hit);
		EndTask();
	}
	else
	{
		OnClientUnconfirmedTargetData.Broadcast(Hit);
	}
	bIsTickable = false;
	EndTask();
}
void UGAAbilityTask_TargetDataLineTrace::Tick(float DeltaTime)
{
	//FHitResult HitOut = LineTrace();
}

FHitResult UGAAbilityTask_TargetDataLineTrace::LineTrace()
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
	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
	World->LineTraceSingleByChannel(HitOut, TraceStart, TraceEnd, CollisionChannel, ColParams, ColResp);
	if (HitOut.bBlockingHit)
	{
		FHitResult NewHit;
		FVector Start = SocketComponent->GetSocketLocation(SocketName);
		FVector NewDir = (HitOut.Location - Start).GetSafeNormal();
		float Distance = FVector::Dist(Start, HitOut.Location);
		FVector End = Start + (NewDir * Range);
		World->LineTraceSingleByChannel(NewHit, Start, End, CollisionChannel, ColParams, ColResp);
		UE_LOG(AbilityFramework, Log, TEXT("UGAAbilityTask_TargetDataLineTrace::LineTrace Corrected"));
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f);
			if (NewHit.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), Start, NewHit.Location, FColor::Magenta, false, 1.0f);
				DrawDebugPoint(GetWorld(), NewHit.Location, 8, FColor::Magenta, false, 1.0f);
			}
		}
	}
	else
	{
		FHitResult NewHit;
		FVector Start = SocketComponent->GetSocketLocation(SocketName);
		FVector NewDir = (TraceEnd - Start).GetSafeNormal();
		float Distance = Range - FVector::Dist(TraceStart, Start);
		FVector End = Start + (NewDir * Distance);

		World->LineTraceSingleByChannel(NewHit, Start, End, CollisionChannel, ColParams, ColResp);
		UE_LOG(AbilityFramework, Log, TEXT("UGAAbilityTask_TargetDataLineTrace::LineTrace"));
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f);
			if (NewHit.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), Start, NewHit.Location, FColor::Magenta, false, 1.0f);
				DrawDebugPoint(GetWorld(), NewHit.Location, 8, FColor::Magenta, false, 1.0f);
			}
		}
	}
	return HitOut;
}
