// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARAbilityWidget.h"
#include "AMTypes.h"
#include "ARAbilityListSlotDragWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityListSlotDragWidget : public UARAbilityWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		TSoftClassPtr<UGAAbilityBase> AbilityTag;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* IconImage;
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry
			, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	inline const TSoftClassPtr<UGAAbilityBase>& GetAbilityTag() const
	{
		return AbilityTag;
	}
	
	void OnItemAdded();
protected:
	void OnItemLoaded(TSoftClassPtr<UGAAbilityBase> InPrimaryAssetId);
};
