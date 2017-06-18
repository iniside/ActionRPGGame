// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARUIGlobals.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ACTIONRPGGAME_API SARFTItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SARFTItem)
	{}
		SLATE_ARGUMENT(FARFTConfig, Config);
		SLATE_ARGUMENT(TWeakObjectPtr<class UARUIComponent>, OwningComp);
	SLATE_END_ARGS()
protected:
	TWeakObjectPtr<class UARUIComponent> OwningComp;

	bool bIsSet;
	float AccumulatedLifeTime;
	FVector2D TransformDirection;
	FWidgetTransform Transform;
	float LifeTime;
	float RandomOffsetX;
	float RandomOffsetY;
	TSharedPtr<STextBlock> TextBlock;
	TAttribute<FText> TextDelegate;
	
public:
	FVector2D Position;
	class SARFTContainer* Container;
	FString Text;
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void Refresh();
	void Reset();
	virtual void Tick(const FGeometry& AllottedGeometry, 
		const double InCurrentTime, const float InDeltaTime) override;
protected:
	FText GetText() const;

};
