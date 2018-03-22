// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARAbilityWidget.h"
#include "AMTypes.h"
#include "ARAbilityListSlotDropWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityListSlotDropWidget : public UARAbilityWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup AbilityGroup;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMSlot AbilitySlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* IconImage;
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry
		, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
