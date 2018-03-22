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
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AssetData;
		FARFilter Filter;
		Filter.TagsAndValues.Add("AbilityTagSearch", AbilityTag.ToString());
		AssetRegistryModule.Get().GetAssets(Filter, AssetData);
		FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("Ability"), AssetData[0].AssetName);
		FPrimaryAssetTypeInfo Info;
		if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
		{
			FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UARAbilityListSlotDragWidget::OnItemLoaded, PrimaryAssetId);

			Manager->LoadPrimaryAsset(PrimaryAssetId,
				TArray<FName>(),
				del);
		}
	}
}

void UARAbilityListSlotDragWidget::OnItemLoaded(FPrimaryAssetId InPrimaryAssetId)
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