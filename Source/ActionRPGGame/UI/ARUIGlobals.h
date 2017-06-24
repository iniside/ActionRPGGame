// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateMaterialBrush.h"
#include "ARUIGlobals.generated.h"
USTRUCT(BlueprintType)
struct FARFTConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FSlateFontInfo Font;
	UPROPERTY(EditAnywhere)
		float XOffsetMin;
	UPROPERTY(EditAnywhere)
		float XOffsetMax;
	UPROPERTY(EditAnywhere)
		float YOffsetMin;
	UPROPERTY(EditAnywhere)
		float YOffsetMax;

	UPROPERTY(EditAnywhere)
		float XDirectionMin;
	UPROPERTY(EditAnywhere)
		float XDirectionMax;
	UPROPERTY(EditAnywhere)
		float YDirectionMin;
	UPROPERTY(EditAnywhere)
		float YDirectionMax;

	UPROPERTY(EditAnywhere)
		float XPositionMin;
	UPROPERTY(EditAnywhere)
		float XPositionMax;
	UPROPERTY(EditAnywhere)
		float YPositionMin;
	UPROPERTY(EditAnywhere)
		float YPositionMax;

	UPROPERTY(EditAnywhere)
		float LifeTimeMin;
	UPROPERTY(EditAnywhere)
		float LifeTimeMax;

	FARFTConfig()
		:XOffsetMin(-2.0f),
		XOffsetMax(2.0f),
		YOffsetMin(-2.0f),
		YOffsetMax(2.0f),
		XDirectionMin(-2.0f),
		XDirectionMax(2.0f),
		YDirectionMin(-2.0f),
		YDirectionMax(2.0f),
		XPositionMin(-20),
		XPositionMax(20),
		YPositionMin(-20),
		YPositionMax(20),
		LifeTimeMin(0.25f),
		LifeTimeMax(0.65f)
	{};
};
USTRUCT(BlueprintType)
struct FARTargetInfoConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FSlateFontInfo Font;
	UPROPERTY(EditAnywhere)
		FSlateColor Color;
	UPROPERTY(EditAnywhere)
		FProgressBarStyle BarStyle;
	UPROPERTY(EditAnywhere)
		FSlateColor BarColor;
	UPROPERTY(EditAnywhere)
		FVector2D BarBorder;
	UPROPERTY(EditAnywhere)
		float MaxWidth;
	UPROPERTY(EditAnywhere)
		float MaxTextHeight;
	UPROPERTY(EditAnywhere)
		float MaxBarHeight;

	UPROPERTY(EditAnywhere)
		float HeightMultiplier;

	UPROPERTY(EditAnywhere)
		FSlateBrush Material;

	FARTargetInfoConfig()
		:HeightMultiplier(1)
	{};
};
/**
 * 
 */
class ACTIONRPGGAME_API ARUIGlobals
{
public:
	ARUIGlobals();
	~ARUIGlobals();
};
