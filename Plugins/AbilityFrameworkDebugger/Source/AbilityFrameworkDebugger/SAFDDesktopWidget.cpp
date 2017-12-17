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

	FOnClicked OnAttributesClickedDel = FOnClicked::CreateSP(this, &SAFDMainWidget::OnAttributesClicked);
	FOnClicked OnEffectsClickedDel = FOnClicked::CreateSP(this, &SAFDMainWidget::OnEffectsClicked);

	ChildSlot
	.HAlign(EHorizontalAlignment::HAlign_Fill)
	.VAlign(EVerticalAlignment::VAlign_Fill)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Top)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(this, &SAFDMainWidget::GetActorName)
				]
				+ SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Top)
				.MaxHeight(24)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SNew(SButton)
						.OnClicked(OnAttributesClickedDel)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Attributes"))
						]
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.OnClicked(OnEffectsClickedDel)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Effects"))
						]
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.VAlign(EVerticalAlignment::VAlign_Fill)
					[
						SAssignNew(Content, SWidgetSwitcher)
					]
				]
			]
		]
		+SVerticalBox::Slot()
		.MaxHeight(32.0f)
			.AutoHeight()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Bottom)
		[
			SNew(SButton)
			.OnClicked(OnPickActorClickedDel)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Pick Actor"))
			]
		]
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

FReply SAFDMainWidget::OnAttributesClicked()
{
	Content->SetActiveWidgetIndex(0);
	return FReply::Handled();
}
FReply SAFDMainWidget::OnEffectsClicked()
{
	Content->SetActiveWidgetIndex(1);
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
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SAFDAttributes)
					.AbilityInterface(Interface)
				];
			Content->AddSlot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SAFDEffects)
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
	wid->SetWindowHandle(Handle);
	return FReply::Handled();
}