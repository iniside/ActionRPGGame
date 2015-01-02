// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GTTraceBase.h"
#include "IGTSocket.h"
#include "GSHUD.h"

AGSHUD::AGSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCTWidget = ObjectInitializer.CreateDefaultSubobject<UFCTHudWidget>(this, TEXT("FCTWidget"));
	bInitialized = false;
}

void AGSHUD::BeginPlay()
{
	Super::BeginPlay();
	if (CrossHairTrace)
		CrossHairTrace->Initialize();
	if (BulletHitTrace)
		BulletHitTrace->Initialize();

	bInitialized = true;
}
void AGSHUD::BeginDestroy()
{
	iSocket = nullptr;
	Super::BeginDestroy();
}
void AGSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bInitialized)
		return;

	if (!OwnChar)
	{
		OwnChar = Cast<ACharacter>(GetOwningPawn());
	}
	if (!iSocket)
	{
		iSocket = Cast<IIGTSocket>(GetOwningPawn());
	}
	if (FCTWidget)
	{
		FCTWidget->HUDPawn = GetOwningPawn();
		FCTWidget->DrawDamageIndicators(Canvas, RenderDelta);
	}
	if (iSocket)
	{
		if (CrossHairTrace)
		{
			FVector HitLocation = CrossHairTrace->GetSingHitLocation();
			FVector ScreenLocation = Project(HitLocation);
			CrossHairPosition.X = ScreenLocation.X;
			CrossHairPosition.Y = ScreenLocation.Y;
		}
		if (BulletHitTrace)
		{
			FVector HitLocation = BulletHitTrace->GetSingHitLocation();
			FVector ScreenLocation = Project(HitLocation);
			HitPosition.X = ScreenLocation.X;
			HitPosition.Y = ScreenLocation.Y;
		}
	}
}

FVector AGSHUD::GetSocketLocation(FName SocketNameIn)
{ 
	return iSocket->GetSocketLocation(SocketNameIn);
}