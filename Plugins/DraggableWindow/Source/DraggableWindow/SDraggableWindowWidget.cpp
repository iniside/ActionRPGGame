// Fill out your copyright notice in the Description page of Project Settings.

#include "SDraggableWindowWidget.h"
#include "SlateOptMacros.h"

#include "Slate.h"
#include "SlateCore.h"
#include "SBorder.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "DWManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "SceneViewport.h"

DECLARE_CYCLE_STAT(TEXT("DraggebleWindow.Tick"), STAT_DraggebleWindowTick, STATGROUP_DraggebleWindow);

FDWWWindowHandle FDWWWindowHandle::Make(TSharedPtr<class SDraggableWindowWidget> InWindow)
{
	static uint32 NewHandle = 0;
	NewHandle++;
	FDWWWindowHandle Handle(NewHandle);
	Handle.Window = InWindow;
	InWindow->SetHandle(Handle);
	return Handle;
}
void SDraggableDesktopWidget::Clean()
{
	for (TSharedPtr<SDraggableWindowWidget>& Window : Windows)
	{
		Window.Reset();
	}
	Windows.Empty();
	Windows.Shrink();
}
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SDraggableDesktopWidget::Construct(const FArguments& InArgs)
{
	SetVisibility(EVisibility::SelfHitTestInvisible);
	Container = SNew(SOverlay).Visibility(EVisibility::SelfHitTestInvisible);
	ChildSlot
	[
		Container.ToSharedRef()
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FDWWWindowHandle SDraggableDesktopWidget::AddWindow(TSharedPtr<class SDraggableWindowWidget> InWindow)
{
	if (!Container.IsValid())
		return FDWWWindowHandle();

	Container->AddSlot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Top)
	[
		InWindow.ToSharedRef()
	];
	
	FDWWWindowHandle Handle = FDWWWindowHandle::Make(InWindow);
	Windows.Add(InWindow);
	return Handle;
}

void SDraggableDesktopWidget::RemoveWindow(TSharedPtr<class SDraggableWindowWidget> InWindow)
{
	Container->RemoveSlot(InWindow.ToSharedRef());
	Windows.Remove(InWindow);
	InWindow.Reset();
}

void SWindowBox::Construct(const FArguments& InArgs)
{
	WidthOverride = InArgs._WidthOverride.Get();
	HeightOverride = InArgs._HeightOverride.Get();

	ChildSlot
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			InArgs._Content.Widget
		];
}
void SWindowBox::SetWidthOverride(float InWidthOverride)
{
	if (WidthOverride != InWidthOverride)
	{
		WidthOverride = InWidthOverride;

		Invalidate(EInvalidateWidget::Layout);
	}
}

void SWindowBox::SetHeightOverride(float InHeightOverride)
{
	if (HeightOverride != InHeightOverride)
	{
		HeightOverride = InHeightOverride;

		Invalidate(EInvalidateWidget::Layout);
	}
}
void SWindowBox::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	const EVisibility& MyCurrentVisibility = this->GetVisibility();
	if (ArrangedChildren.Accepts(MyCurrentVisibility))
	{
		const FMargin SlotPadding(ChildSlot.SlotPadding.Get());
		bool bAlignChildren = true;

		AlignmentArrangeResult XAlignmentResult(0, 0);
		AlignmentArrangeResult YAlignmentResult(0, 0);

		if (bAlignChildren)
		{
			XAlignmentResult = AlignChild<Orient_Horizontal>(AllottedGeometry.GetLocalSize().X, ChildSlot, SlotPadding);
			YAlignmentResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, ChildSlot, SlotPadding);
		}

		const float AlignedSizeX = XAlignmentResult.Size;
		const float AlignedSizeY = YAlignmentResult.Size;

		ArrangedChildren.AddWidget(
			AllottedGeometry.MakeChild(
				ChildSlot.GetWidget(),
				FVector2D(XAlignmentResult.Offset, YAlignmentResult.Offset),
				FVector2D(AlignedSizeX, AlignedSizeY)
			)
		);
	}
}
FChildren* SWindowBox::GetChildren()
{
	return &ChildSlot;
}
FVector2D SWindowBox::ComputeDesiredSize(float) const
{
	EVisibility ChildVisibility = ChildSlot.GetWidget()->GetVisibility();

	if (ChildVisibility != EVisibility::Collapsed)
	{
		// If the user specified a fixed width or height, those values override the Box's content.
		
		const float CurrentWidthOverride = WidthOverride;
		const float CurrentHeightOverride = HeightOverride;


		return FVector2D(CurrentWidthOverride, CurrentHeightOverride);
	}

	return FVector2D::ZeroVector;
}
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SDraggableWindowWidget::Construct(const FArguments& InArgs)
{
	CurrentSize = FVector2D(1, 1);
	ResizingState = EDDWState::NoResize;
	bDestroyOnClose = true;
	SetVisibility(EVisibility::SelfHitTestInvisible);
	FSimpleDelegate OnPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnPressed);
	FSimpleDelegate OnReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnReleased);
	TAttribute<FVector2D> posAttr = TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateSP(this, &SDraggableWindowWidget::GetPosition));


	FSimpleDelegate OnHorizontalPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnHorizontalResizePressed);
	FSimpleDelegate OnHorizontalReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnHorizontalResizeReleased);
	FSimpleDelegate OnHorizontalLeftPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnHorizontalResizeLeftPressed);
	FSimpleDelegate OnHorizontalLeftReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnHorizontalResizeLeftReleased);

	FSimpleDelegate OnVerticalTopPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnVerticalTopResizePressed);
	FSimpleDelegate OnVerticalTopReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnVerticalTopResizeReleased);
	FSimpleDelegate OnVerticalBottomPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnVerticalBottomResizePressed);
	FSimpleDelegate OnVerticalBottomReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnVerticalBottomResizeReleased);

	FSimpleDelegate OnBottomRightPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnBottomRightResizePressed);
	FSimpleDelegate OnBottomRightReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnBottomRightResizeReleased);

	FSimpleDelegate OnBottomLeftPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnBottomLeftResizePressed);
	FSimpleDelegate OnBottomLeftReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnBottomLeftResizeReleased);

	FSimpleDelegate OnTopRightPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnTopRightResizePressed);
	FSimpleDelegate OnTopRightReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnTopRightResizeReleased);
	
	FSimpleDelegate OnTopLeftPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnTopLeftResizePressed);
	FSimpleDelegate OnTopLeftReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnTopLeftResizeReleased);


	FSimpleDelegate OnCloseButtonPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnCloseButtonPressed);

	CurrentHeight = 200;
	CurrentWidth = 200;

	FSlateBrush brush;
	brush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
	ButtonStyle.Normal = brush;
	ButtonStyle.Hovered = brush;
	ButtonStyle.Pressed = brush;
	ButtonStyle.Disabled = brush;
	BackgroundColor = FSlateColor(FLinearColor(0, 0, 0, 1));
	ChildSlot
	[
		SNew(SCanvas)
		+ SCanvas::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.Position(posAttr)
		//.Size(sizeAttr)
		[
			SNew(SGridPanel)
			
			+SGridPanel::Slot(0, 0)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[	
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnTopLeftPressedDel)
					.OnReleased(OnTopLeftReleasedDel)
					.Cursor(EMouseCursor::ResizeSouthEast)
				]
			]
			+ SGridPanel::Slot(1 ,0)
			[
				SNew(SBox)
				.HeightOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnVerticalTopPressedDel)
					.OnReleased(OnVerticalTopReleasedDel)
					.Cursor(EMouseCursor::ResizeUpDown)
					
				]
			]
			+ SGridPanel::Slot(2, 0)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnTopRightPressedDel)
					.OnReleased(OnTopRightReleasedDel)
					.Cursor(EMouseCursor::ResizeSouthWest)
				]
			]
			+ SGridPanel::Slot(0, 1)
			[
				SNew(SBox)
				.WidthOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnHorizontalLeftPressedDel)
					.OnReleased(OnHorizontalLeftReleasedDel)
					.Cursor(EMouseCursor::ResizeLeftRight)
				]
			]
			+ SGridPanel::Slot(1, 1)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SAssignNew(WindowBox, SWindowBox)
				.Visibility(EVisibility::SelfHitTestInvisible)
				//.HeightOverride(heightAttr)
				//.WidthOverride(widthAttr)
				[
					SNew(SVerticalBox)
					.Visibility(EVisibility::SelfHitTestInvisible)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.MaxHeight(24)
					[
						SNew(SOverlay)
						+ SOverlay::Slot()
						[
							SNew(SBackgroundBlur)
							.BlurRadius(4)
							.BlurStrength(16)
							.Visibility(EVisibility::SelfHitTestInvisible)
						]
						+ SOverlay::Slot()
						[
							SAssignNew(WindowBar, SButton)
							.OnPressed(OnPressedDel)
							.OnReleased(OnReleasedDel)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.ContentPadding(FMargin(0))
							.ButtonStyle(&ButtonStyle)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(0.8f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Visibility(EVisibility::SelfHitTestInvisible)
									.Text(FText::FromString("Window Title"))
								]
								+ SHorizontalBox::Slot()
								[
									SNew(SButton)
									.OnPressed(OnCloseButtonPressedDel)
									[
										SNew(STextBlock)
										.Text(FText::FromString("X"))
									]
								]
							]
						]
						
					]
					+ SVerticalBox::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.VAlign(EVerticalAlignment::VAlign_Fill)
					.FillHeight(1.0f)
					[
						SNew(SBox)
						.HAlign(EHorizontalAlignment::HAlign_Fill)
						.VAlign(EVerticalAlignment::VAlign_Fill)
						[
							SNew(SOverlay)
							+ SOverlay::Slot()
							[
								SNew(SBackgroundBlur)
								.BlurRadius(4)
								.BlurStrength(8)
								.Visibility(EVisibility::SelfHitTestInvisible)
							]
							+SOverlay::Slot()
							[
								SAssignNew(Content, SOverlay)
								.Visibility(EVisibility::SelfHitTestInvisible)
							]
							
						]
					]
				]
				
			]
			+ SGridPanel::Slot(2, 1)
			[
				SNew(SBox)
				.WidthOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnHorizontalPressedDel)
					.OnReleased(OnHorizontalReleasedDel)
					.Cursor(EMouseCursor::ResizeLeftRight)
				]
			]
			+ SGridPanel::Slot(0, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnBottomLeftPressedDel)
					.OnReleased(OnBottomLeftReleasedDel)
					.Cursor(EMouseCursor::ResizeSouthWest)
				]
			]
			+ SGridPanel::Slot(1, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnVerticalBottomPressedDel)
					.OnReleased(OnVerticalBottomReleasedDel)
					.Cursor(EMouseCursor::ResizeUpDown)
				]
			]
			+ SGridPanel::Slot(2, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
					.ButtonStyle(&ButtonStyle)
					.OnPressed(OnBottomRightPressedDel)
					.OnReleased(OnBottomRightReleasedDel)
					.Cursor(EMouseCursor::ResizeSouthEast)
				]
			]
		]
	];
	WindowBox->SetHeightOverride(CurrentHeight);
	WindowBox->SetWidthOverride(CurrentWidth);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
SDraggableWindowWidget::~SDraggableWindowWidget()
{
}
FVector2D SDraggableWindowWidget::ComputeDesiredSize(float) const
{
	return SCompoundWidget::ComputeDesiredSize(1);
}
void SDraggableWindowWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_DraggebleWindowTick);
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	FVector2D LastPosition = AllottedGeometry.AbsoluteToLocal(FSlateApplicationBase::Get().GetLastCursorPos());
	
	FVector2D CurrentPosition = AllottedGeometry.AbsoluteToLocal(FSlateApplicationBase::Get().GetCursorPos());
	
	float dist = FVector2D::Distance(CurrentPosition, LastPosition);
	
	switch (ResizingState)
	{
	case EDDWState::Dragging:
	{
		FVector2D AbsPos = FSlateApplicationBase::Get().GetCursorPos();
		
		CurrentPosition = CurrentPosition - DragPosition;
		AbsPosition = AbsPos - AbsDragPosition;

		FVector2D WindowPosition = GEngine->GameViewport->GetWindow()->GetPositionInScreen();
		FVector2D WindowSize = GEngine->GameViewport->GetWindow()->GetSizeInScreen() - 8;
		
		const float ApplicationScale = FSlateApplication::Get().GetApplicationScale();
		FVector2D AbsSize = AllottedGeometry.LocalToAbsolute(FVector2D(CurrentWidth, CurrentHeight));
		FVector2D GeomAbs = AllottedGeometry.GetAbsolutePosition();
		FVector2D localSize = WindowSize + WindowPosition;
		CurrentCursorPosition = CurrentPosition;
		if (CurrentPosition.X <= 0)
		{
			CurrentCursorPosition.X = 0;
		}
		if (CurrentPosition.Y <= 0)
		{
			CurrentCursorPosition.Y = 0;
		}
		if ((AbsPos.X + ((AbsSize.X - AbsDragPosition.X) * ApplicationScale)) >= localSize.X)
		{
			FVector2D localSize2 = AllottedGeometry.AbsoluteToLocal(WindowSize + WindowPosition);
			CurrentCursorPosition.X = localSize2.X - CurrentWidth;
		}
		if ((AbsPos.Y + ((AbsSize.Y - AbsDragPosition.Y) * ApplicationScale)) >= localSize.Y)
		{
			FVector2D localSize2 = AllottedGeometry.AbsoluteToLocal(WindowSize + WindowPosition);
			CurrentCursorPosition.Y = localSize2.Y - CurrentHeight;
		}
		break;
	}
	case EDDWState::HorizontalRight:
	{	
		float CurrentXX = 0;
		if (dist != 0)
		{
			CurrentXX = CurrentPosition.X - (CurrentCursorPosition.X + CurrentWidth);
		}
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetWidthOverride(CurrentWidth);
		break;
	}
	case EDDWState::HorizontalLeft:
	{
		float CurrentXX = 0;
		if (dist != 0)
		{
			CurrentXX = CurrentCursorPosition.X - CurrentPosition.X;
		}
		
		CurrentCursorPosition.X -= CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetWidthOverride(CurrentWidth);
		break;
	}
	case EDDWState::VerticalTop:
	{	
		float CurrentXX = 0;
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentXX = CurrentCursorPosition.Y - CurrentPosition.Y;
			
		}
		CurrentCursorPosition.Y -= CurrentXX;
		CurrentHeight = CurrentHeight + CurrentXX;
		WindowBox->SetHeightOverride(CurrentHeight);
		break;
	}
	case EDDWState::VerticalBottom:
	{	
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
		break;
	}
	case EDDWState::DiagonalBottomRight:
	{	
		float CurrentXX = 0;
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentXX = CurrentPosition.X - (CurrentCursorPosition.X + CurrentWidth);
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		CurrentWidth = CurrentWidth + CurrentXX;
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
		break;
	}
	case EDDWState::DiagonalBottomLeft:
	{	
		float CurrentXX = 0;
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentXX = CurrentCursorPosition.X - CurrentPosition.X;
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		CurrentCursorPosition.X -= CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
		break;
	}
	case EDDWState::DiagonalTopRight:
	{	
		float CurrentXX = 0;
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentY = CurrentCursorPosition.Y - CurrentPosition.Y;
			CurrentXX = CurrentPosition.X - (CurrentCursorPosition.X + CurrentWidth);
		}
		CurrentCursorPosition.Y -= CurrentY;
		
		CurrentHeight = CurrentHeight + CurrentY;
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
		break; 
	}
	case EDDWState::DiagonalTopLeft:
	{	
		float CurrentXX = 0;
		float CurrentY = 0;
		if (dist != 0)
		{
			CurrentY = CurrentCursorPosition.Y - CurrentPosition.Y;
			CurrentXX = CurrentCursorPosition.X - CurrentPosition.X;
		}
		CurrentCursorPosition.Y -= CurrentY;
		CurrentCursorPosition.X -= CurrentXX;
		CurrentHeight = CurrentHeight + CurrentY;
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
		break;
	}
	case EDDWState::NoResize:
	{	break;
	}
	default:
		break;
	}
}
void SDraggableWindowWidget::AddContent(TSharedPtr<SWidget> InWidget)
{
	Content->AddSlot()
	.HAlign(EHorizontalAlignment::HAlign_Fill)
	.VAlign(EVerticalAlignment::VAlign_Fill)
	[
		InWidget.ToSharedRef()
	];
}
void SDraggableWindowWidget::SetHandle(const FDWWWindowHandle& InHandle)
{
	Handle = InHandle;
}
void SDraggableWindowWidget::OnCloseButtonPressed()
{
	if(bDestroyOnClose)
		FDWManager::Get().RemoveWindow(Handle);
	else
	{
		SetVisibility(EVisibility::Collapsed);
	}
}
void SDraggableWindowWidget::OnPressed()
{
	FGeometry geom = GetCachedGeometry();
	FVector2D CurrentPosAbs = FSlateApplicationBase::Get().GetLastCursorPos();
	FVector2D CurrentPosition = geom.AbsoluteToLocal(CurrentPosAbs);
	//UE_LOG(LogTemp, Warning, TEXT("SDraggableWindowWidget::OnPressed Pre DragPosition X: %f Y: %f"), DragPosition.X, DragPosition.Y);
	DragPosition = CurrentPosition - CurrentCursorPosition;
	AbsDragPosition = CurrentPosAbs - AbsPosition;

	//UE_LOG(LogTemp, Warning, TEXT("SDraggableWindowWidget::OnPressed CurrentPosition X: %f Y: %f"), CurrentPosition.X, CurrentPosition.Y);
	//UE_LOG(LogTemp, Warning, TEXT("SDraggableWindowWidget::OnPressed CurrentCursorPosition X: %f Y: %f"), CurrentCursorPosition.X, CurrentCursorPosition.Y);
	//UE_LOG(LogTemp, Warning, TEXT("SDraggableWindowWidget::OnPressed DragPosition X: %f Y: %f"), DragPosition.X, DragPosition.Y);
	ResizingState = EDDWState::Dragging;
}
void SDraggableWindowWidget::OnReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnHorizontalResizePressed()
{
	ResizingState = EDDWState::HorizontalRight;
}
void SDraggableWindowWidget::OnHorizontalResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}
void SDraggableWindowWidget::OnHorizontalResizeLeftPressed()
{
	ResizingState = EDDWState::HorizontalLeft;
}
void SDraggableWindowWidget::OnHorizontalResizeLeftReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnVerticalTopResizePressed()
{
	ResizingState = EDDWState::VerticalTop;
}
void SDraggableWindowWidget::OnVerticalTopResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}
void SDraggableWindowWidget::OnVerticalBottomResizePressed()
{
	ResizingState = EDDWState::VerticalBottom;
}
void SDraggableWindowWidget::OnVerticalBottomResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnDirectionalResizePressed()
{
}

void SDraggableWindowWidget::OnDirectionalResizeReleased()
{
}

void SDraggableWindowWidget::OnBottomRightResizePressed()
{
	ResizingState = EDDWState::DiagonalBottomRight;
}
void SDraggableWindowWidget::OnBottomRightResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnBottomLeftResizePressed()
{
	ResizingState = EDDWState::DiagonalBottomLeft;
}
void SDraggableWindowWidget::OnBottomLeftResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnTopRightResizePressed()
{
	ResizingState = EDDWState::DiagonalTopRight;
}
void SDraggableWindowWidget::OnTopRightResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}

void SDraggableWindowWidget::OnTopLeftResizePressed()
{
	ResizingState = EDDWState::DiagonalTopLeft;
}
void SDraggableWindowWidget::OnTopLeftResizeReleased()
{
	ResizingState = EDDWState::NoResize;
}

FVector2D SDraggableWindowWidget::GetPosition() const
{
	return CurrentCursorPosition;
}

FText SDraggableWindowWidget::GetTitle() const
{
	return WindowTitle;
}