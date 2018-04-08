// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityListSlotDragWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ARAbilityDragVisual.h"
#include "Abilities/ARAbilityManagerComponent.h"
#include "Abilities/ARAbilityBase.h"
#include "Abilities/ARAbilityUIData.h"

FReply UARAbilityListSlotDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARAbilityListSlotDragWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(AbilityManager->GetOwner());
		//UARAbilityDragVisual* DragIcon = CreateWidget<UARAbilityDragVisual>(MyPC, AbilityManager->GetDragVisualClass());
		//DragIcon->AbilityManager = AbilityManager;

		DragDropOp->Payload = this;
		DragDropOp->DefaultDragVisual = IconImage;

		OutOperation = DragDropOp;
	}
}

void UARAbilityListSlotDragWidget::OnItemAdded()
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	{
		FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UARAbilityListSlotDragWidget::OnItemLoaded, AbilityTag);

		Manager.RequestAsyncLoad(AbilityTag.ToSoftObjectPath()
			, del);
	}
}

void UARAbilityListSlotDragWidget::OnItemLoaded(TSoftClassPtr<UGAAbilityBase> InPrimaryAssetId)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	{
		TSubclassOf<UARAbilityBase> AbilityClass = InPrimaryAssetId.Get();
		if (AbilityClass)
		{
			IconImage->SetBrushFromTexture(AbilityClass.GetDefaultObject()->UIData->Icon);
		}

		{
			Manager.Unload(InPrimaryAssetId.ToSoftObjectPath());
		}
	}
}