// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ACTIONRPGGAME_API SARTargetInfo : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SARTargetInfo)
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<class APlayerController>, PC);
		SLATE_ARGUMENT(TWeakObjectPtr<class UARUIComponent>, UIComp);
	SLATE_END_ARGS()
protected:
	TWeakObjectPtr<class APlayerController> PC;
	TWeakObjectPtr<class UARUIComponent> UIComp;
	TWeakObjectPtr<AActor> Target;
	TAttribute<FText> TextDelegate;
	TAttribute<TOptional<float>> PercentDelegate;
	TSharedPtr<STextBlock> TextBlk;
	TSharedPtr<SProgressBar> HealthBar;
	class IIGAAbilities* ABInterface;
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry,
		const double InCurrentTime, const float InDeltaTime) override;
	void Show(AActor* InTarget);
	void Hide();

protected:
	FText GetText() const;
	TOptional<float> GetPercentage() const;
	TPair<float, float> GetObjectScreenRadius(AActor* InActor);
};
