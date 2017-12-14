// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDDesktopWidget.h"
#include "SlateCore.h"
#include "Slate.h"
#include "SlateOptMacros.h"
#include "SBoxPanel.h"
#include "SButton.h"
#include "STextBlock.h"
#include "AFDManager.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "SAFDViewportMouseCapture.h"
#include "SAFDEffects.h"
#include "SAFDAttributes.h"

void SAFDMainWidget::Construct(const FArguments& InArgs)
{
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* world = WorldContext->World();
	World = world;
	FOnClicked OnPickActorClickedDel = FOnClicked::CreateSP(this, &SAFDMainWidget::OnPickActorClicked);

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SAssignNew(Content, SOverlay)
		]
		+SVerticalBox::Slot()
		[
			SNew(SButton)
			.OnClicked(OnPickActorClickedDel)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Pick Actor"))
			]
		]
	];

	Content->AddSlot()
	.HAlign(EHorizontalAlignment::HAlign_Left)
	.VAlign(EVerticalAlignment::VAlign_Top)
	[
		SNew(STextBlock)
		.Text(this, &SAFDMainWidget::GetActorName)
	];
}

FText SAFDMainWidget::GetActorName() const
{
	if(SelectedActor.IsValid())
	{
		return FText::FromString(SelectedActor->GetName());
	}
	return FText::FromString("Select Actor from viewport.");
}
FReply SAFDMainWidget::OnPickActorClicked()
{
	CaptureWidget = SNew(SAFDViewportMouseCapture);
	FSimpleDelegate del = FSimpleDelegate::CreateSP(this, &SAFDMainWidget::OnActorPicked);
	CaptureWidget->OnMouseButtonDownDelegate = del;
	GEngine->GameViewport->AddViewportWidgetContent(CaptureWidget.ToSharedRef(), 1001);

	return FReply::Handled();
}
void SAFDMainWidget::OnActorPicked()
{
	APlayerController* PC = World->GetFirstPlayerController();
	FHitResult OutHit;
	PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, OutHit);

	if(OutHit.bBlockingHit)
	{
		AActor* actor = OutHit.GetActor();
		SelectedActor = OutHit.Actor;
		if(IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(OutHit.GetActor()))
		{
			Content->AddSlot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Top)
				[
					SNew(SAFDAttributes)
					.AbilityInterface(Interface)
				];
		}
	}

	GEngine->GameViewport->RemoveViewportWidgetContent(CaptureWidget.ToSharedRef());
}
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDDesktopWidget::Construct(const FArguments& InArgs)
{
	FOnClicked OnNewDebugWindowClickedDel = FOnClicked::CreateSP(this, &SAFDDesktopWidget::OnNewDebugWindowClicked);
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			[
				SNew(SButton)
				.OnClicked(OnNewDebugWindowClickedDel)
				[
					SNew(STextBlock)
					.Text(FText::FromString("New Debug Window"))
				]
			]
		]
		// Populate the widget
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SAFDDesktopWidget::OnNewDebugWindowClicked()
{
	TSharedPtr<SDraggableWindowWidget> window = SNew(SDraggableWindowWidget);
	FDWWWindowHandle Handle = FAFDManager::Get().AddNewWindow(window);
	TSharedPtr<SAFDMainWidget> wid = SNew(SAFDMainWidget);
	window->AddContent(wid);
	wid->WindowHandle = Handle;
	return FReply::Handled();
}