// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GTTraceBase.h"
#include "IGTSocket.h"
#include "GSCharacter.h"
#include "Weapons/GSWeaponRanged.h"
#include "Components/GSActiveActionsComponent.h"

#include "Items/GSItemWeaponRangedInfo.h"

#include "GSHUD.h"
const int32 FGSCrosshairDirection::Left = 0;
const int32 FGSCrosshairDirection::Right = 1;
const int32 FGSCrosshairDirection::Top = 2;
const int32 FGSCrosshairDirection::Bottom = 3;
const int32 FGSCrosshairDirection::Center = 4;

AGSHUD::AGSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCTWidget = ObjectInitializer.CreateDefaultSubobject<UFCTHudWidget>(this, TEXT("FCTWidget"));
	bInitialized = false;
	CurrentSpread = 1;
}
void AGSHUD::SetCurrentSpread(float CurrentSpreadIn)
{
	CurrentSpread = CurrentSpreadIn;
}
void AGSHUD::BeginPlay()
{
	Super::BeginPlay();
	if (CrossHairTrace)
		CrossHairTrace->Initialize();
	if (BulletHitTrace)
		BulletHitTrace->Initialize();
	if (CrossHairTexture)
	{
		Crosshair[FGSCrosshairDirection::Left] = UCanvas::MakeIcon(CrossHairTexture, 43, 402, 25, 9); // left
		Crosshair[FGSCrosshairDirection::Right] = UCanvas::MakeIcon(CrossHairTexture, 88, 402, 25, 9); // right
		Crosshair[FGSCrosshairDirection::Top] = UCanvas::MakeIcon(CrossHairTexture, 74, 371, 9, 25); // top
		Crosshair[FGSCrosshairDirection::Bottom] = UCanvas::MakeIcon(CrossHairTexture, 74, 415, 9, 25); // bottom
		Crosshair[FGSCrosshairDirection::Center] = UCanvas::MakeIcon(CrossHairTexture, 75, 403, 7, 7); // center
	}
	bInitialized = true;
}
void AGSHUD::DrawCrosshair()
{
	FCanvasIcon* CurrentCrosshair[5];
	for (int32 i = 0; i< 5; i++)
	{
		CurrentCrosshair[i] = &Crosshair[i];
	}
	float CenterX = CrossHairPosition.X;// Canvas->ClipX / 2;
	float CenterY = CrossHairPosition.Y;// Canvas->ClipY / 2;
	float Scale = 1;
	float FinalSpread = CrossSpread * CurrentSpread;
	Canvas->DrawIcon(*CurrentCrosshair[FGSCrosshairDirection::Center],
		CenterX - (*CurrentCrosshair[FGSCrosshairDirection::Center]).UL * CrosshairScale / 2.0f,
		CenterY - (*CurrentCrosshair[FGSCrosshairDirection::Center]).VL * CrosshairScale / 2.0f, CrosshairScale);

	Canvas->DrawIcon(*CurrentCrosshair[FGSCrosshairDirection::Left],
		CenterX - 1 - (*CurrentCrosshair[FGSCrosshairDirection::Left]).UL * CrosshairScale - FinalSpread * CrosshairScale,
		CenterY - (*CurrentCrosshair[FGSCrosshairDirection::Left]).VL * CrosshairScale / 2.0f, CrosshairScale);
	Canvas->DrawIcon(*CurrentCrosshair[FGSCrosshairDirection::Right],
		CenterX + FinalSpread * CrosshairScale,
		CenterY - (*CurrentCrosshair[FGSCrosshairDirection::Right]).VL * CrosshairScale / 2.0f, CrosshairScale);

	Canvas->DrawIcon(*CurrentCrosshair[FGSCrosshairDirection::Top],
		CenterX - (*CurrentCrosshair[FGSCrosshairDirection::Top]).UL * CrosshairScale / 2.0f,
		CenterY - 1 - (*CurrentCrosshair[FGSCrosshairDirection::Top]).VL * CrosshairScale - FinalSpread * CrosshairScale, CrosshairScale);
	Canvas->DrawIcon(*CurrentCrosshair[FGSCrosshairDirection::Bottom],
		CenterX - (*CurrentCrosshair[FGSCrosshairDirection::Bottom]).UL * CrosshairScale / 2.0f,
		CenterY + FinalSpread * CrosshairScale, CrosshairScale);
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
		OwnChar = Cast<AGSCharacter>(GetOwningPawn());
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
	if (OwnChar) //(iSocket)
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
		if (CurrentLeftWeapon != OwnChar->ActiveActions->CurrentLeftHandWeapon)
		{
			if (UGSItemWeaponRangedInfo* weap = Cast<UGSItemWeaponRangedInfo>(OwnChar->ActiveActions->CurrentLeftHandWeapon))
			{
				//weap->RangedWeapon->OnCurrentWeaponSpread.Clear();
				//weap->RangedWeapon->OnCurrentWeaponSpread.AddUObject(this, &AGSHUD::SetCurrentSpread);
				CurrentSpread = weap->RangedWeapon->GetCurrentSpread();
			}
		}
	}
	DrawCrosshair();
}

FVector AGSHUD::GetSocketLocation(FName SocketNameIn)
{ 
	//return OwnChar->GetStartLocationForCrosshair();
	return iSocket->GetSocketLocation(SocketNameIn);
}