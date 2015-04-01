// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWidgets.h"
#include "FCTFloatingTextComponent.h"

#include "Widgets/FCTBaseWidget.h"
#include "Widgets/FCTFloatingWidget.h"
//will have single struct param, with data about how attribute changed.


UFCTFloatingTextComponent::UFCTFloatingTextComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	SetComponentTickEnabled(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UFCTFloatingTextComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (int32 Index = 0; Index < MaximumIndicators; Index++)
	{
		FFCTDisplayData data;
		data.DisplayText = FText::FromString("");
		data.TargetLocation = FVector::ZeroVector;
		data.FadeTime = 0;

		CachedDisplayData.Add(data);
	}

	UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
	//if (FloatingTextType)
	//{
	//	for (int32 Index = 0; Index < MaximumIndicators; Index++)
	//	{
	//		UFCTFloatingWidget* floatingWidget = ConstructObject<UFCTFloatingWidget>(FloatingTextType, Outer);
	//		if (floatingWidget)
	//		{
	//			ULocalPlayer* Player = GetWorld()->GetFirstLocalPlayerFromController(); //temporary
	//			floatingWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
	//			//floatingWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	//			floatingWidget->Initialize();
	//			//IndicatorContainer->OwningComponent = this;
	//			floatingWidget->LifeTime = 0;
	//			//IndicatorContainer->MaximumIndicators = MaximumIndicators;
	//			floatingWidget->PCOwner = PCOwner;
	//			floatingWidget->AddToViewport();
	//			//IndicatorContainer->InitializeWidget();
	//			floatingWidget->SetVisibility(ESlateVisibility::Hidden);
	//			
	//			FloatingWidgets.Add(floatingWidget);
	//		}
	//	}
	//}
	OnReceivedData.AddUObject(this, &UFCTFloatingTextComponent::RecivedData);
	SetComponentTickEnabled(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	RegisterAllComponentTickFunctions(true);
}
void UFCTFloatingTextComponent::RecivedData(const FFCTDisplayData& DataIn)
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
	//FloatingWidgets[BestIndex]->DisplayData.TargetLocation += TextOffset;
	//FloatingWidgets[BestIndex]->SetVisibility(ESlateVisibility::HitTestInvisible);
}
void UFCTFloatingTextComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//for (UFCTFloatingWidget* widget : FloatingWidgets)
	//{
	//	widget->MyTick(DeltaTime);
	//}
}