// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponListSlotDragWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ARAbilityDragVisual.h"
#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARWeaponManagerComponent.h"

FReply UARWeaponListSlotDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARWeaponListSlotDragWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(WeaponManager->GetOwner());
		//UARAbilityDragVisual* DragIcon = CreateWidget<UARAbilityDragVisual>(MyPC, AbilityManager->GetDragVisualClass());
		//DragIcon->AbilityManager = AbilityManager;

		DragDropOp->Payload = this;
		//DragDropOp->DefaultDragVisual = DragIcon;

		OutOperation = DragDropOp;
	}
}

void UARWeaponListSlotDragWidget::OnItemAdded()
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	{
		TSoftClassPtr<UGAAbilityBase> AbilityTag = WeaponManager->WeaponClasses[WeaponIdx].GetDefaultObject()->Ability;
		
		{
			FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UARWeaponListSlotDragWidget::OnItemLoaded, AbilityTag);

			Manager.RequestAsyncLoad(AbilityTag.ToSoftObjectPath()
				, del);
		}
	}
}

void UARWeaponListSlotDragWidget::OnItemLoaded(TSoftClassPtr<UGAAbilityBase> InPrimaryAssetId)
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