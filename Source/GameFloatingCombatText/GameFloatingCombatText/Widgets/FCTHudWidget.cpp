// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameFloatingCombatText.h"

#include "../FCTFloatingTextComponent.h"

#include "FCTHudWidget.h"

UFCTHudWidget::UFCTHudWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnReceivedData.AddUObject(this, &UFCTHudWidget::PawnDamaged);
	for (int32 Index = 0; Index < 10; Index++)
	{
		FFCTDamageHudIndicator dmgInd;
		dmgInd.FadeTime = 0;
		dmgInd.AnimDirection = FVector2D(-1, -1);
		dmgInd.CurrentLocation = FVector2D::ZeroVector;
		DamageIndicators.Add(dmgInd);
	}
}

void UFCTHudWidget::PawnDamaged(const FFCTDisplayData& UIDamage)
{
	// Calculate the rotation 	
	//AUTCharacter* UTC = UTPlayerOwner->GetUTCharacter();
	if (HUDPawn)	// If have a pawn and it's alive...
	{
		FVector CharacterLocation;
		FRotator CharacterRotation;

		HUDPawn->GetActorEyesViewPoint(CharacterLocation, CharacterRotation);
		FVector HitSafeNormal = (UIDamage.TargetLocation - CharacterLocation).GetSafeNormal2D();
		float Ang = FMath::Acos(FVector::DotProduct(CharacterRotation.Vector().GetSafeNormal2D(), HitSafeNormal)) * (180.0f / PI);

		// Figure out Left/Right....
		float FinalAng = (FVector::DotProduct(FVector::CrossProduct(CharacterRotation.Vector(), FVector(0, 0, 1)), HitSafeNormal)) > 0 ? 360 - Ang : Ang;

		int BestIndex = 0;
		float BestTime = DamageIndicators[0].FadeTime;
		for (int i = 0; i < 10; i++)
		{
			if (DamageIndicators[i].FadeTime <= 0.0f)
			{
				BestIndex = i;
				DamageIndicators[i].CurrentLocation = FVector2D::ZeroVector;
				DamageIndicators[i].AnimDirection = FVector2D::ZeroVector;
				break;
			}
			else
			{
				if (DamageIndicators[i].FadeTime < BestTime)
				{
					BestIndex = i;
					BestTime = DamageIndicators[i].FadeTime;
				}
			}
		}

		DamageIndicators[BestIndex].FadeTime = FCTSettings.FCTLifeTime;
		DamageIndicators[BestIndex].RotationAngle = FinalAng;
		DamageIndicators[BestIndex].DisplayText = UIDamage.DisplayText;
		DamageIndicators[BestIndex].Location = UIDamage.TargetLocation;
		DamageIndicators[BestIndex].AnimDirection.X = FMath::FRandRange(FCTSettings.AnimationXMin, FCTSettings.AnimationXMax);
		DamageIndicators[BestIndex].AnimDirection.Y = FMath::FRandRange(FCTSettings.AnimationYMin, FCTSettings.AnimationYMax);
		//		UUTHUDWidget_WeaponCrosshair* CrossHairWidget =


	}
}
//
void UFCTHudWidget::DrawDamageIndicators(UCanvas* CanvasIn, float DeltaTime)
{
	FLinearColor DrawColor = FLinearColor::White;
	FVector2D vec2d;

	for (int i = 0; i < DamageIndicators.Num(); i++)
	{
		if (DamageIndicators[i].FadeTime > 0.0f)
		{
			DrawColor.A = 1.0 * (DamageIndicators[i].FadeTime / FCTSettings.FCTLifeTime);

			float Size = 384 * (CanvasIn->ClipY / 720.0f);
			float Half = Size * 0.5;
			FVector vecTemp = CanvasIn->Project(DamageIndicators[i].Location);
			DamageIndicators[i].CurrentLocation.X += DamageIndicators[i].AnimDirection.X;
			DamageIndicators[i].CurrentLocation.Y += DamageIndicators[i].AnimDirection.Y;
			vec2d.X += FMath::FRandRange(-FCTSettings.RandomOffset, FCTSettings.RandomOffset);
			vec2d.Y += FMath::FRandRange(-FCTSettings.RandomOffset, FCTSettings.RandomOffset);
			vec2d.X = vecTemp.X + DamageIndicators[i].CurrentLocation.X;
			vec2d.Y = vecTemp.Y + DamageIndicators[i].CurrentLocation.Y;
			//FVector2D((Canvas->ClipX * 0.5) - Half, (Canvas->ClipY * 0.5) - Half)
			FCanvasTextItem TextItem = FCanvasTextItem(vec2d, FText::FromString(DamageIndicators[i].DisplayText), FCTSettings.FontType, FCTSettings.FontColor);

			TextItem.Scale = FVector2D(FCTSettings.FontScale, FCTSettings.FontScale);

			CanvasIn->DrawItem(TextItem);

			DamageIndicators[i].FadeTime -= DeltaTime;
		}
	}
}
