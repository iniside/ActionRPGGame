// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARUIGlobals.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ACTIONRPGGAME_API SARFTContainer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SARFTContainer)
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<class APlayerController>, PC);
		SLATE_ARGUMENT(TWeakObjectPtr<class UARUIComponent>, OwningComp);
		SLATE_ARGUMENT(int32, PoolSize);
		SLATE_ARGUMENT(FARFTConfig, Config);
	SLATE_END_ARGS()
protected:
	TWeakObjectPtr<class APlayerController> OwningPC;

public:
	TQueue<TSharedPtr<class SARFTItem>> ItemsPool;
	TQueue<TSharedPtr<class SARFTItem>> UsedItemsPool;
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void EmitText(FVector WorldPos, const FString& InText);
	void Reset();
};
