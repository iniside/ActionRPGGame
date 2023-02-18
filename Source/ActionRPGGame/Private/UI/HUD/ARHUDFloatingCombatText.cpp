// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUDFloatingCombatText.h"
#include "Engine/Engine.h"

#include "SlateOptMacros.h"

#include "Slate.h"
#include "SlateCore.h"

#include "Engine/GameViewportClient.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "ARPlayerController.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SFCTWidget::Construct(const FArguments& InArgs)
{

	TAttribute<FSlateFontInfo> fontInfo = TAttribute<FSlateFontInfo>::Create(TAttribute<FSlateFontInfo>::FGetter::CreateSP(this, &SFCTWidget::GetFontInfo));
	ChildSlot
		[
			SNew(STextBlock).Text(this, &SFCTWidget::GetText).Font(fontInfo)
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SFCTWidget::~SFCTWidget()
{

}
FSlateFontInfo SFCTWidget::GetFontInfo() const
{
	return Font;
}
FText SFCTWidget::GetText() const
{
	return Text;
}

void UARHUDFloatingCombatText::Init(int32 Num)
{
	
	Canvas = SNew(SConstraintCanvas);
	GEngine->GameViewport->AddViewportWidgetContent(Canvas.ToSharedRef());
	for (int32 Idx = 0; Idx < Num; Idx++)
	{
		FARHUDFCTUpdate Update;
		Update.Widget = SNew(SFCTWidget);
		Update.Widget->Font = Font;
		
		Update.Slot = &Canvas->AddSlot()
			[
				Update.Widget.ToSharedRef()
			];
		Update.Widget->SetVisibility(EVisibility::Collapsed);
		Widgets.Add(Update);
	}
}

void UARHUDFloatingCombatText::Update(float InDeltaTime)
{
	for (uint8 Idx = 0; Idx < Widgets.Num(); Idx++)
	{
		if (Widgets[Idx].bUsed)
		{
			Widgets[Idx].CurrentTime += InDeltaTime;
			Widgets[Idx].CurrentPosition = Widgets[Idx].CurrentPosition + (Widgets[Idx].Direction * InDeltaTime * FCTMoveSpeed);
			
			float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
			
			FMargin Pos(Widgets[Idx].CurrentPosition.X / Scale, Widgets[Idx].CurrentPosition.Y / Scale, Widgets[Idx].Widget->GetDesiredSize().X, Widgets[Idx].Widget->GetDesiredSize().Y);
			Widgets[Idx].Slot->Offset(Pos);

			if (Widgets[Idx].CurrentTime > FCTLifeTime)
			{
				Widgets[Idx].bUsed = false;
				Widgets[Idx].CurrentTime = 0;
				
				Widgets[Idx].Widget->SetVisibility(EVisibility::Collapsed);

			}
		}
	}
}

void UARHUDFloatingCombatText::SetInfo(float InDamage, FVector2D ScreenPosition, FLinearColor TextColor)
{
	for (uint8 Idx = 0; Idx < Widgets.Num(); Idx++)
	{
		if (!Widgets[Idx].bUsed)
		{
			Widgets[Idx].bUsed = true;
			Widgets[Idx].CurrentTime = 0;
			Widgets[Idx].CurrentPosition = ScreenPosition;
			float RandomX = FMath::FRandRange(-1, 1);
			float RandomY = FMath::FRandRange(-1, 1);
			Widgets[Idx].Direction = FVector2D(RandomX, RandomY);
			Widgets[Idx].Widget->TextColor = FSlateColor(TextColor);
			Widgets[Idx].Widget->SetText(FText::AsNumber(InDamage));
			Widgets[Idx].Widget->SetVisibility(EVisibility::HitTestInvisible);

			break;
		}
	}
}