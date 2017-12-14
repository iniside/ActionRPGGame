// Fill out your copyright notice in the Description page of Project Settings.

#include "SDraggableWindowWidget.h"
#include "SlateOptMacros.h"

#include "Slate.h"
#include "SlateCore.h"
#include "SBorder.h"
#include "Widgets/Layout/SGridPanel.h"

FDWWWindowHandle FDWWWindowHandle::Make(TSharedPtr<class SDraggableWindowWidget> InWindow)
{
	static uint32 NewHandle = 0;
	NewHandle++;
	FDWWWindowHandle Handle(NewHandle);
	Handle.Window = InWindow;
	return NewHandle;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SDraggableDesktopWidget::Construct(const FArguments& InArgs)
{
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
	return Handle;
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
		//const FOptionalSize CurrentMaxAspectRatio = MaxAspectRatio.Get();
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
				//FVector2D(WidthOverride.Get().Get(), HeightOverride.Get().Get())
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

	FSimpleDelegate OnPressedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnPressed);
	FSimpleDelegate OnReleasedDel = FSimpleDelegate::CreateSP(this, &SDraggableWindowWidget::OnReleased);
	TAttribute<FVector2D> posAttr = TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateSP(this, &SDraggableWindowWidget::GetPosition));
	TAttribute<FVector2D> sizeAttr = TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateSP(this, &SDraggableWindowWidget::GetSize));


	TAttribute<FOptionalSize> widthAttr = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateSP(this, &SDraggableWindowWidget::GetWidth));
	TAttribute<FOptionalSize> heightAttr = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateSP(this, &SDraggableWindowWidget::GetHeight));

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

	CurrentHeight = 200;
	CurrentWidth = 200;

	bDragging = false;

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
				]
			]
			+ SGridPanel::Slot(1 ,0)
			[
				SNew(SBox)
				.HeightOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnVerticalTopPressedDel)
					.OnReleased(OnVerticalTopReleasedDel)
				]
			]
			+ SGridPanel::Slot(2, 0)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
				]
			]
			+ SGridPanel::Slot(0, 1)
			[
				SNew(SBox)
				.WidthOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnHorizontalLeftPressedDel)
					.OnReleased(OnHorizontalLeftReleasedDel)
				]
			]
			+ SGridPanel::Slot(1, 1)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SAssignNew(WindowBox, SWindowBox)
				/*.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)*/
				//.HeightOverride(heightAttr)
				//.WidthOverride(widthAttr)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					//.FillHeight(0.2f)
					.MaxHeight(32)
					.MaxHeight(32)
					[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							[
								SNew(SButton)
								.OnPressed(OnPressedDel)
								.OnReleased(OnReleasedDel)
							]
						
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SAssignNew(Content, SOverlay)
					]
				]
				
			]
			+ SGridPanel::Slot(2, 1)
			[
				SNew(SBox)
				.WidthOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnHorizontalPressedDel)
					.OnReleased(OnHorizontalReleasedDel)
				]
			]
			+ SGridPanel::Slot(0, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnBottomLeftPressedDel)
					.OnReleased(OnBottomLeftReleasedDel)
				]
			]
			+ SGridPanel::Slot(1, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnVerticalBottomPressedDel)
					.OnReleased(OnVerticalBottomReleasedDel)
				]
			]
			+ SGridPanel::Slot(2, 2)
			[
				SNew(SBox)
				.HeightOverride(3)
				.WidthOverride(3)
				[
					SNew(SButton)
					.OnPressed(OnBottomRightPressedDel)
					.OnReleased(OnBottomRightReleasedDel)
				]
			]
		]
		// Populate the widget
	];
	WindowBox->SetHeightOverride(CurrentHeight);
	WindowBox->SetWidthOverride(CurrentWidth);
	/*Content->AddSlot()
		[
			SNew(SButton)
			[
				SNew(STextBlock)
				.Text(this, &SDraggableWindowWidget::GetCurrentX)
			]
		];*/

}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
FVector2D SDraggableWindowWidget::ComputeDesiredSize(float) const
{
	return SCompoundWidget::ComputeDesiredSize(1);
	//return FVector2D(CurrentWidth, CurrentHeight);
}
void SDraggableWindowWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	FVector2D LastPosition = AllottedGeometry.AbsoluteToLocal(FSlateApplicationBase::Get().GetLastCursorPos());
	FVector2D CurrentPosition = AllottedGeometry.AbsoluteToLocal(FSlateApplicationBase::Get().GetCursorPos());
	float dist = FVector2D::Distance(CurrentPosition, LastPosition);// *InDeltaTime;
	
	if (bDragging)
	{
		FVector2D locaPos = AllottedGeometry.AbsoluteToLocal(FSlateApplicationBase::Get().GetCursorPos());
		CurrentCursorPosition = locaPos;
	}
	if(ResizingState == EDDWState::HorizontalRight)
	{
		float CurrentXX = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		if(dist != 0)
		{
			CurrentXX = CurrentPosition.X - (CurrentCursorPosition.X + CurrentWidth);
		}
		CurrentX = CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetWidthOverride(CurrentWidth);
	}
	else if (ResizingState == EDDWState::HorizontalLeft)
	{
		float CurrentXX = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		if (dist != 0)
		{
			CurrentXX = CurrentCursorPosition.X - CurrentPosition.X;
		}
		//float CurrentXX = LastPosition.X - CurrentPosition.X;
		CurrentCursorPosition.X -= CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		WindowBox->SetWidthOverride(CurrentWidth);
	}
	else if(ResizingState == EDDWState::VerticalTop)
	{
		
	}
	else if (ResizingState == EDDWState::VerticalBottom)
	{
		float CurrentY = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		if (dist != 0)
		{
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		//CurrentX = CurrentY;
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
	}
	else if(ResizingState == EDDWState::DiagonalBottomRight)
	{
		float CurrentXX = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		float CurrentY = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		if (dist != 0)
		{
			CurrentXX = CurrentPosition.X - (CurrentCursorPosition.X + CurrentWidth);
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		CurrentX = CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
	}
	else if(ResizingState == EDDWState::DiagonalBottomLeft)
	{
		float CurrentXX = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		float CurrentY = 0;// dist * sign;// CurrentPosition.X - LastPosition.X;
		if (dist != 0)
		{
			CurrentXX = CurrentCursorPosition.X - CurrentPosition.X;
			CurrentY = CurrentPosition.Y - (CurrentCursorPosition.Y + CurrentHeight);
		}
		CurrentX = CurrentXX;
		CurrentCursorPosition.X -= CurrentXX;
		CurrentWidth = CurrentWidth + CurrentXX;
		CurrentHeight = CurrentHeight + CurrentY;
		WindowBox->SetHeightOverride(CurrentHeight);
		WindowBox->SetWidthOverride(CurrentWidth);
	}
	
}
void SDraggableWindowWidget::AddContent(TSharedPtr<SWidget> InWidget)
{
	Content->AddSlot()
	[
		InWidget.ToSharedRef()
	];
}
void SDraggableWindowWidget::OnPressed()
{
	bDragging = true;
	//CurrentCursorPosition = FSlateApplication::Get().GetCursorPos();
}
void SDraggableWindowWidget::OnReleased()
{
	bDragging = false;
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

FVector2D SDraggableWindowWidget::GetPosition() const
{
	return CurrentCursorPosition;
}
FVector2D SDraggableWindowWidget::GetSize() const
{
	return CurrentSize;
}
FOptionalSize SDraggableWindowWidget::GetHeight() const
{
	return CurrentHeight;
}
FOptionalSize SDraggableWindowWidget::GetWidth() const
{
	return CurrentWidth;
}
FText SDraggableWindowWidget::GetCurrentX() const
{
	FString ret = FString("Right") + FString::FormatAsNumber(CurrentSize.X) + FString("\n")
		+ FString("CurrentWidth ") + FString::FormatAsNumber(CurrentWidth) + FString("\n")
		+ FString("CurrentX ") + FString::FormatAsNumber(CurrentX) + FString("\n");
	return FText::FromString(ret);
}
