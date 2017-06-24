// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "SARTargetInfo.h"
#include "ARUIComponent.h"
#include "ARUIGlobals.h"
#include "WidgetLayoutLibrary.h"
#include "IGAAbilities.h"
#include "Attributes/GAAttributeBase.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARTargetInfo::Construct(const FArguments& InArgs)
{
	PC = InArgs._PC;
	UIComp = InArgs._UIComp;

	const FARTargetInfoConfig& Config = InArgs._UIComp->TargetInfoConfig;
	
	TextDelegate.Bind(this, &SARTargetInfo::GetText);
	PercentDelegate.Bind(this, &SARTargetInfo::GetPercentage);
	ChildSlot
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.MaxWidth(Config.MaxWidth)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.MaxHeight(Config.MaxTextHeight)
				[
					SAssignNew(TextBlk, STextBlock)
					.Text(TextDelegate)
					.Font(Config.Font)
					.ColorAndOpacity(Config.Color)
				]
				+SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.MaxHeight(Config.MaxBarHeight)
				[
					SAssignNew(HealthBar, SProgressBar)
					.BorderPadding(Config.BarBorder)
					.Style(&Config.BarStyle)
					.FillColorAndOpacity(Config.BarColor)
					.BarFillType(EProgressBarFillType::LeftToRight)
					.Percent(PercentDelegate)
				]
			]
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SARTargetInfo::Tick(const FGeometry& AllottedGeometry,
	const double InCurrentTime, const float InDeltaTime)
{
	if (!Target.IsValid())
		return;
	if (GetVisibility() == EVisibility::HitTestInvisible)
	{
		const FARTargetInfoConfig& Config = UIComp->TargetInfoConfig;
		FVector WorldPos = Target->GetActorLocation();
		FVector OutPosition;

		FBox BB = Target->GetComponentsBoundingBox();
		FVector Center = BB.GetCenter();
		FVector Extent = BB.GetExtent() * Config.HeightMultiplier;
		FVector2D Center2D;// = FVector2D(Canvas->Project(FVector(Center.X, Center.Y, Center.Z + Extent.Z)));
		float ActorExtent = 40;
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPositionWithDistance(PC.Get(), 
			FVector(Center.X, Center.Y, Center.Z + Extent.Z), OutPosition);
		FWidgetTransform Transform;
		FVector2D Size = ComputeDesiredSize(1) * 0.25f;// TextBlk->ComputeDesiredSize(1) * 0.5f;
		
		TPair<float, float> distSize = GetObjectScreenRadius(Target.Get());
		float NormalizedVal = FMath::GetMappedRangeValueClamped(FVector2D(3500, 0), FVector2D(0.35, 1), OutPosition.Z);
		
		Size.X = Config.MaxWidth * NormalizedVal * 0.5f ;
		FVector2D WidgetPos(OutPosition.X, OutPosition.Y);
		
		//float NormalizedScale = FMath::GetMappedRangeValueClamped(FVector2D(0, 200), FVector2D(0.35, 1), distSize.Value);
		//WidgetPos.Y -= WidgetPos.Y*distSize.Value;
		//WidgetPos.Y = (distSize.Value*Config.HeightMultiplier);

		Transform.Translation = WidgetPos - Size;
		Transform.Scale = FVector2D(1, 1);
		//Transform.Scale.Y *= NormalizedVal;
		//Transform.Scale.X *= NormalizedVal*0.8f;
		SetRenderTransform(Transform.ToSlateRenderTransform());
	}
}
void SARTargetInfo::Show(AActor* InTarget)
{
	if (IIGAAbilities* AbilityInterface = Cast<IIGAAbilities>(InTarget))
	{
		//if (Target != InTarget)
		{
			Target = InTarget;
			ABInterface = AbilityInterface;
			SetVisibility(EVisibility::HitTestInvisible);
		}
	}
	else
	{
		SetVisibility(EVisibility::Collapsed);
	}
}
void SARTargetInfo::Hide()
{
	Target.Reset();
	SetVisibility(EVisibility::Collapsed);
}
FText SARTargetInfo::GetText() const
{
	if (Target.IsValid())
	{
		return FText::FromString(Target->GetName());
	}
	return FText::FromString("");
}
TOptional<float> SARTargetInfo::GetPercentage() const
{
	float Percent = 0;
	if (ABInterface)
	{
		if (FAFAttributeBase* Attr = ABInterface->GetAttribute(FGAAttribute("Health")))
		{
			float MaxVal = Attr->GetFinalValue();
			float Current = Attr->GetCurrentValue();

			Percent = FMath::GetMappedRangeValueClamped(FVector2D(0, MaxVal), FVector2D(0, 1), Current);
		}
	}
	return Percent;
}
#if WITH_EDITOR
/* Get Screen Percentage */
static const auto CVarScreenPercentage = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.SCreenPercentage"));
#endif WITH_EDITOR

TPair<float, float> SARTargetInfo::GetObjectScreenRadius(AActor* InActor)
{
	float ScreenRadius;
	int32 Width, Height;
	FVector Viewlocation;
	FRotator ViewRotation; // Not Used, but required for Function call
	float CamFOV = 90.0f; //TODO: Replace With Function that returns camera FOV
#if WITH_EDITOR
	float ScreenPerc = CVarScreenPercentage->GetValueOnGameThread() / 100.0f;
#endif WITH_EDITOR

	/* Get the size of the viewport, and the player cameras location. */
	PC->GetViewportSize(Width, Height);
	PC->GetPlayerViewPoint(Viewlocation, ViewRotation);

#if WITH_EDITOR
	/* Factor in Screen Percentage & Quality Settings */
	Width *= ScreenPerc;
	Height *= ScreenPerc;
#endif WITH_EDITOR

	/* Easy Way To Return The Size, Create a vector and scale it. Alternative would be to use FMath::Max3 */
	float SRad = FVector2D(Width, Height).Size();

	/* Get Object Bounds (R) */
	float BoundingRadius = InActor->GetRootComponent()->Bounds.SphereRadius;
	float DistanceToObject = FVector(InActor->GetActorLocation() - Viewlocation).Size();

	/* Get Projected Screen Radius */
	ScreenRadius = FMath::Atan(BoundingRadius / DistanceToObject);
	//ScreenRadius *= FMath::DegreesToRadians(CamFOV) * SRad;
	ScreenRadius *= SRad / FMath::DegreesToRadians(CamFOV);
	TPair<float, float> retVal(DistanceToObject, ScreenRadius);

	return retVal;
}