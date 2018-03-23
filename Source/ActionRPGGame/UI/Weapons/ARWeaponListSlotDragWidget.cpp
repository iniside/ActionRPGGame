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
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		FGameplayTag AbilityTag = WeaponManager->WeaponClasses[WeaponIdx].GetDefaultObject()->Ability;
		TArray<FAssetData> AssetData;
		FARFilter Filter;
		Filter.TagsAndValues.Add("AbilityTagSearch", AbilityTag.ToString());
		AssetRegistryModule.Get().GetAssets(Filter, AssetData);
		FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("Ability"), AssetData[0].AssetName);
		FPrimaryAssetTypeInfo Info;
		if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
		{
			FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UARWeaponListSlotDragWidget::OnItemLoaded, PrimaryAssetId);

			Manager->LoadPrimaryAsset(PrimaryAssetId,
				TArray<FName>(),
				del);
		}
	}
}

void UARWeaponListSlotDragWidget::OnItemLoaded(FPrimaryAssetId InPrimaryAssetId)
{
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* loaded = Manager->GetPrimaryAssetObject(InPrimaryAssetId);
		TSubclassOf<UARAbilityBase> AbilityClass = Cast<UClass>(loaded);
		if (AbilityClass)
		{
			IconImage->SetBrushFromTexture(AbilityClass.GetDefaultObject()->UIData->Icon);
		}

		{
			Manager->UnloadPrimaryAsset(InPrimaryAssetId);
		}
	}
}