// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GSHUD.h"

AGSHUD::AGSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCTWidget = ObjectInitializer.CreateDefaultSubobject<UFCTHudWidget>(this, TEXT("FCTWidget"));
}


void AGSHUD::DrawHUD()
{
	Super::DrawHUD();
	if (FCTWidget)
	{
		FCTWidget->HUDPawn = GetOwningPawn();
		FCTWidget->DrawDamageIndicators(Canvas, RenderDelta);
	}
}