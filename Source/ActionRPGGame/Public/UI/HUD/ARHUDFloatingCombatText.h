// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Queue.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/SCanvas.h"
#include "Widgets/SCompoundWidget.h"
#include "Components/TextBlock.h"

#include "ARHUDFloatingCombatText.generated.h"
UCLASS()
class ACTIONRPGGAME_API UARFCTText : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Text;

	void SetText(const FText& InText)
	{
		Text->SetText(InText);
	}
};

class SFCTWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SFCTWidget) {}
	SLATE_END_ARGS()

public:
	FSlateFontInfo Font;
	FText Text;
	FSlateColor TextColor;
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	~SFCTWidget();
	inline void SetText(const FText& InText) { Text = InText; };
	FSlateFontInfo GetFontInfo() const;
	FText GetText() const;
};

struct FARHUDFCTUpdate
{
	
	TSharedPtr<SFCTWidget> Widget;

	float CurrentTime;
	bool bUsed;
	FVector2D CurrentPosition;
	FVector2D Direction;
	SConstraintCanvas::FSlot* Slot;
	FARHUDFCTUpdate()
		: CurrentTime(0)
		, bUsed(false)
		, CurrentPosition(FVector2D::ZeroVector)
		, Direction(FVector2D::ZeroVector)
	{};
};

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARHUDFloatingCombatText : public UUserWidget
{
	GENERATED_BODY()
		friend class AARHUD;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UARFCTText> UARFCTTextClass;
	UPROPERTY()
		class AARPlayerController* PC;

	float FCTMoveSpeed;
	float FCTLifeTime;
	/* Pool of available widgets */
	TQueue<FARHUDFCTUpdate> UnsedWidgtes;
	/* Pool of used Widgets which are updateing. */
	TArray<FARHUDFCTUpdate> Widgets;

	TSharedPtr<SConstraintCanvas> Canvas;

	FSlateFontInfo Font;

	
public:
	void Init(int32 Num);

	void Update(float InDeltaTime);

	void SetInfo(float InDamage, FVector2D ScreenPosition, FLinearColor TextColor = FLinearColor::White);

};
