// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "SARFTItem.h"
#include "SARFTContainer.h"
#include "ARUIComponent.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARFTItem::Construct(const FArguments& InArgs)
{
	OwningComp = InArgs._OwningComp;
	FARFTConfig& Config = OwningComp->FloatingTextConfig;
	TextDelegate.Bind(this, &SARFTItem::GetText);
	ChildSlot
	[
		SAssignNew(TextBlock, STextBlock)
		.Text(TextDelegate)
		.ColorAndOpacity(FSlateColor(FLinearColor::Red))
		.Font(Config.Font)
	];
	
}

void SARFTItem::Tick(const FGeometry& AllottedGeometry, 
	const double InCurrentTime, const float InDeltaTime)
{
	if ((GetVisibility() == EVisibility::HitTestInvisible))
	{
		if(!bIsSet)
		{
			bIsSet = true;
		}
		TransformDirection.X = TransformDirection.X - (RandomOffsetX * InDeltaTime);
		TransformDirection.Y = TransformDirection.Y - (RandomOffsetY * InDeltaTime);

		Transform.Translation += TransformDirection;

		SetRenderTransform(Transform.ToSlateRenderTransform());

		UE_LOG(LogTemp, Warning, TEXT("Your message %f"), TransformDirection.X);
		AccumulatedLifeTime += InDeltaTime;
		if (AccumulatedLifeTime > LifeTime)
		{
			if (Container)
			{
				TSharedPtr<SARFTItem> Item;
				Container->UsedItemsPool.Dequeue(Item);
				Container->ItemsPool.Enqueue(Item);
				
			}
			SetVisibility(EVisibility::Collapsed);
		}
	}
}

void SARFTItem::Refresh()
{
	bIsSet = false;
	FARFTConfig& Config = OwningComp->FloatingTextConfig;
	LifeTime = FMath::FRandRange(Config.LifeTimeMin, Config.LifeTimeMax);
	RandomOffsetX = FMath::FRandRange(Config.XOffsetMin, Config.XOffsetMax);
	RandomOffsetY = FMath::FRandRange(Config.YOffsetMin, Config.YOffsetMax);
	AccumulatedLifeTime = 0;
	float OffsetX = FMath::FRandRange(Config.XPositionMin, Config.XPositionMax);
	float OffsetY = FMath::FRandRange(Config.YPositionMin, Config.YPositionMax);
	FVector2D Size = TextBlock->ComputeDesiredSize(1) * 0.5f;
	Transform.Translation = (Position - Size) + FVector2D(OffsetX, OffsetY);
	float XDirection = FMath::FRandRange(Config.XDirectionMin, Config.XDirectionMax);
	float YDirection = FMath::FRandRange(Config.YDirectionMin, Config.YDirectionMax);
	TransformDirection = FVector2D(XDirection, YDirection);

	SetRenderTransform(Transform.ToSlateRenderTransform());
	SetVisibility(EVisibility::HitTestInvisible);
}
void SARFTItem::Reset()
{
	
}
FText SARFTItem::GetText() const
{
	return FText::FromString(Text);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
