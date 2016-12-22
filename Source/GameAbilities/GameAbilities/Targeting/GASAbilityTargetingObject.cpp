// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../GAAbilityBase.h"
#include "GASAbilityTargetingObject.h"

FHitResult UGASAbilityTargetingObject::GetTarget()
{
	FHitResult HitOut;
	APlayerController* PC = AbilityOwner->PCOwner;
	APawn* P = AbilityOwner->POwner;
	FVector TraceStart = PC->PlayerCameraManager->GetCameraLocation(); // P->GetPawnViewLocation();
	FRotator UnusedRot = P->GetBaseAimRotation();
	PC->GetActorEyesViewPoint(TraceStart, UnusedRot);
	FVector TraceEnd = UnusedRot.Vector() * Range + TraceStart;
	UWorld* World = GetWorld();
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(P);
	FCollisionResponseParams ColResp;
	
	World->LineTraceSingleByChannel(HitOut, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, ColParams, ColResp);
	if (HitOut.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, HitOut.ImpactPoint, FColor::Red, true, 4);
	}
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, true, 4);

	return HitOut;
}

class UWorld* UGASAbilityTargetingObject::GetWorld() const
{
	if (AbilityOwner.IsValid())
		return AbilityOwner->GetWorld();
	return nullptr;
}
