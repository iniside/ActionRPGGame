// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameFloatingCombatText.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "FCTFloatingWidget.h"
#include "../FCTFloatingTextComponent.h"

#include "FCTBaseWidget.h"

UFCTBaseWidget::UFCTBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UFCTBaseWidget::InitializeWidget()
{
	if (OwningComponent)
	{
		OwningComponent->OnReceivedData.AddUObject(this, &UFCTBaseWidget::RecivedData);

		//UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
		//if (FloatingTextType)
		//{
		//	for (int32 Index = 0; Index < MaximumIndicators; Index++)
		//	{
		//		UFCTFloatingWidget* floatingWidget = ConstructObject<UFCTFloatingWidget>(FloatingTextType, Outer);
		//		if (floatingWidget)
		//		{
		//			ULocalPlayer* Player = GetWorld()->GetFirstLocalPlayerFromController(); //temporary
		//			floatingWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
		//			floatingWidget->Initialize();
		//			//IndicatorContainer->OwningComponent = this;
		//			floatingWidget->LifeTime = 0;
		//			//IndicatorContainer->MaximumIndicators = MaximumIndicators;
		//			floatingWidget->PCOwner = PCOwner;
		//			//IndicatorContainer->InitializeWidget();
		//			floatingWidget->SetVisibility(ESlateVisibility::Hidden);
		//			FloatingWidgets.Add(floatingWidget);
		//		}
		//	}
		//}
	}
}

void UFCTBaseWidget::RecivedData(const FFCTDisplayData& DataIn)
{
	//int32 BestIndex = 0;
	//float BestTime = FloatingWidgets[0]->LifeTime;

	//for (int32 Index = 0; Index < MaximumIndicators; Index++)
	//{
	//	if (FloatingWidgets[Index]->LifeTime <= 0.0f)
	//	{
	//		BestIndex = Index;
	//		break;
	//	}
	//	else
	//	{
	//		if (FloatingWidgets[Index]->LifeTime < BestTime)
	//		{
	//			BestIndex = Index;
	//			BestTime = FloatingWidgets[Index]->LifeTime;
	//		}
	//	}
	//}

	//FloatingWidgets[BestIndex]->LifeTime = FloatingTextLifeTime;
	//FloatingWidgets[BestIndex]->DisplayData = DataIn;
	//FloatingWidgets[BestIndex]->SetVisibility(ESlateVisibility::HitTestInvisible);
}

//void UFCTBaseWidget::Tick_Implementation(FGeometry MyGeometry, float InDeltaTime)
//{
//	Super::Tick(MyGeometry, InDeltaTime);
//	for (UFCTFloatingWidget* widget : FloatingWidgets)
//	{
//		widget->MyTick(InDeltaTime);
//	}
//}

void UFCTBaseWidget::TickMe(float DeltaTimeIn)
{

}