// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/Events.h"
#include "Widgets/SCompoundWidget.h"
#include "SlateCore.h"
#include "Styling/SlateTypes.h"
#include "SOverlay.h"
#include "SGridPanel.h"
#include "SBox.h"
#include "SBorder.h"
#include "SButton.h"
#include "SConstraintCanvas.h"
#include "DWTypes.h"

DECLARE_STATS_GROUP(TEXT("DraggebleWindow"), STATGROUP_DraggebleWindow, STATCAT_Advanced);

enum class EDDWState : uint8
{
	Dragging = 0,
	HorizontalRight = 1,
	HorizontalLeft = 2,
	VerticalTop = 3,
	VerticalBottom = 4,
	DiagonalBottomRight = 5,
	DiagonalBottomLeft = 6,
	DiagonalTopRight = 7,
	DiagonalTopLeft = 8,

	NoResize
};
class DRAGGABLEWINDOW_API SDraggableDesktopWidget : public SCompoundWidget
{

	SLATE_BEGIN_ARGS(SDraggableDesktopWidget) {}
	SLATE_END_ARGS()
public:
	friend class FDWManager;
	friend class SDraggableWindowWidget;
		/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	TArray<TSharedPtr<class SDraggableWindowWidget>> Windows;
	TSharedPtr<class SOverlay> Container;
	void Clean();
	FDWWWindowHandle AddWindow(TSharedPtr<class SDraggableWindowWidget> InWindow);
	void RemoveWindow(TSharedPtr<class SDraggableWindowWidget> InWindow);
};

class SWindowBox : public SPanel
{
public:
	class FBoxSlot : public TSupportsOneChildMixin<FBoxSlot>, public TSupportsContentAlignmentMixin<FBoxSlot>, public TSupportsContentPaddingMixin<FBoxSlot>
	{
	public:
		FBoxSlot(SWidget* InOwner)
			: TSupportsOneChildMixin<FBoxSlot>(nullptr)
			, TSupportsContentAlignmentMixin<FBoxSlot>(HAlign_Fill, VAlign_Fill)
		{
		}
	};
	SLATE_BEGIN_ARGS(SWindowBox)
		: _Content()
		, _WidthOverride()
		, _HeightOverride()
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		/** When specified, ignore the content's desired size and report the WidthOverride as the Box's desired width. */
		SLATE_ATTRIBUTE(float, WidthOverride)

		/** When specified, ignore the content's desired size and report the HeightOverride as the Box's desired height. */
		SLATE_ATTRIBUTE(float, HeightOverride)
	SLATE_END_ARGS()
private:
	/** When specified, ignore the content's desired size and report the.WidthOverride as the Box's desired width. */
	float WidthOverride;

	/** When specified, ignore the content's desired size and report the.HeightOverride as the Box's desired height. */
	float HeightOverride;
protected:

	FBoxSlot ChildSlot;
public:
	SWindowBox()
		: ChildSlot(this)
	{}
	void Construct(const FArguments& InArgs);
		/** See WidthOverride attribute */
	void SetWidthOverride(float InWidthOverride);

	/** See HeightOverride attribute */
	void SetHeightOverride(float InHeightOverride);
protected:
	/**
	* Panels arrange their children in a space described by the AllottedGeometry parameter. The results of the arrangement
	* should be returned by appending a FArrangedWidget pair for every child widget. See StackPanel for an example
	*
	* @param AllottedGeometry    The geometry allotted for this widget by its parent.
	* @param ArrangedChildren    The array to which to add the WidgetGeometries that represent the arranged children.
	*/
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;

	/**
	* All widgets must provide a way to access their children in a layout-agnostic way.
	* Panels store their children in Slots, which creates a dilemma. Most panels
	* can store their children in a TPanelChildren<Slot>, where the Slot class
	* provides layout information about the child it stores. In that case
	* GetChildren should simply return the TPanelChildren<Slot>. See StackPanel for an example.
	*/
	virtual FChildren* GetChildren() override;
};

/**
 * Check IF cursor is moving during resizing.
 */
class DRAGGABLEWINDOW_API SDraggableWindowWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDraggableWindowWidget){}
		SLATE_ATTRIBUTE(bool, HideOnClose)
	SLATE_END_ARGS()
public:
	friend struct FDWWWindowHandle;
	friend class SDraggableDesktopWidget;
	friend class FDWManager;
	bool bDestroyOnClose;
protected:
	EDDWState ResizingState;
	
	FVector2D CurrentSize;
	FVector2D CurrentCursorPosition;
	FVector2D DragPosition;
	FVector2D AbsPosition;
	FVector2D AbsDragPosition;

	TSharedPtr<SOverlay> Content;
	TSharedPtr<SWindowBox> WindowBox;
	float CurrentHeight;
	float CurrentWidth;
	FButtonStyle ButtonStyle;
	TAttribute<FSlateColor> BackgroundColor;
	TSharedPtr<SButton> WindowBar;
	FDWWWindowHandle Handle;

	FText WindowTitle;
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	~SDraggableWindowWidget();
protected:
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	void AddContent(TSharedPtr<SWidget> InWidget);

	void SetHandle(const FDWWWindowHandle& InHandle);
	void OnCloseButtonPressed();

	void OnPressed();
	void OnReleased();

	void OnHorizontalResizePressed();
	void OnHorizontalResizeReleased();
	void OnHorizontalResizeLeftPressed();
	void OnHorizontalResizeLeftReleased();

	void OnVerticalTopResizePressed();
	void OnVerticalTopResizeReleased();
	void OnVerticalBottomResizePressed();
	void OnVerticalBottomResizeReleased();

	void OnDirectionalResizePressed();
	void OnDirectionalResizeReleased();

	void OnBottomRightResizePressed();
	void OnBottomRightResizeReleased();

	void OnBottomLeftResizePressed();
	void OnBottomLeftResizeReleased();

	void OnTopRightResizePressed();
	void OnTopRightResizeReleased();

	void OnTopLeftResizePressed();
	void OnTopLeftResizeReleased();

	FVector2D GetPosition() const;

	FText GetTitle() const;
};
