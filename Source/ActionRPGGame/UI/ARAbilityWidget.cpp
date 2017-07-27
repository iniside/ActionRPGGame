// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityWidget.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARUIAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"

#include "ARAbilityBase.h"
#include "ARAbilityUIData.h"
#include "Input/Reply.h"
#include "Input/Events.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ARGlobals.h"

float UARAbilityWidget::GetActivationRemainingTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetActivationRemainingTime() : 0;
}
float UARAbilityWidget::GetActivationRemainingTimeNormalized()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetActivationRemainingTimeNormalized() : 0;
}
float UARAbilityWidget::GetActivationCurrentTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetActivationCurrentTime() : 0;
}
float UARAbilityWidget::GetActivationCurrentTimeNormalized()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetActivationCurrentTimeNormalized() : 0;
}
float UARAbilityWidget::GetActivationEndTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetActivationEndTime() : 0;
}

float UARAbilityWidget::GetCooldownRemainingTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetCooldownRemainingTime() : 0;
}
float UARAbilityWidget::GetCooldownRemainingTimeNormalized()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetCooldownRemainingTimeNormalized() : 0;
}
float UARAbilityWidget::GetCooldownCurrentTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetCooldownCurrentTime() : 0;
}
float UARAbilityWidget::GetCooldownCurrentTimeNormalized()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetCooldownCurrentTimeNormalized() : 0;
}
float UARAbilityWidget::GetCooldownEndTime()
{
	if (!OwningComponent)
		return 0;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->GetCooldownEndTime() : 0;
}
UTexture2D* UARAbilityWidget::GetIcon()
{
	if (Icon)
		return Icon;

	if (!OwningComponent)
		return nullptr;
	UARAbilityBase* Ability = OwningComponent->GetAbility(AbilitySetIndex, AbilityIndex);
	return Ability ? Ability->UIData->Icon : nullptr;
}

void UARAbilityWidget::Setbility(const FGameplayTag& InAbility)
{
	AbilityTag = InAbility;
	FAssetData AssetData = FARGlobals::GetAbilityAssetByTag(AbilityTag);
	TSubclassOf<UARAbilityBase> cls;// = nullptr;
	if (AssetData.IsValid())
	{
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			FPrimaryAssetId PrimaryAssetId = FARGlobals::MakeAbilityAssetId(AssetData);
			FPrimaryAssetTypeInfo Info;
			if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))// && !Info.bHasBlueprintClasses)
			{
				cls = Manager->GetPrimaryAssetObjectClass<UARAbilityBase>(PrimaryAssetId);
				FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UARAbilityWidget::OnFinishedLoad, PrimaryAssetId);
				Manager->LoadPrimaryAsset(PrimaryAssetId,
					TArray<FName>(),
					del);
			}
		}
	}
}
void UARAbilityWidget::OnFinishedLoad(FPrimaryAssetId PrimaryAssetId)
{
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* loaded = Manager->GetPrimaryAssetObject(PrimaryAssetId);
		TSubclassOf<UARAbilityBase> cls = Cast<UClass>(loaded);
		if (cls)
		{
			UARAbilityBase* CDO = cls.GetDefaultObject();
			Icon = CDO->UIData->Icon;
		}

		{
			Manager->UnloadPrimaryAsset(PrimaryAssetId);
		}
	}
}
FReply UARAbilityWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARAbilityWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(OwningComponent->GetOwner());
		UARAbilityWidget* DragIcon = CreateWidget<UARAbilityWidget>(MyPC, OwningComponent->DragVisualClass);
		DragIcon->AbilityIndex = AbilityIndex;
		DragIcon->AbilitySetIndex = AbilitySetIndex;
		DragIcon->OwningComponent = OwningComponent;

		DragDropOp->Payload = this;
		DragDropOp->DefaultDragVisual = DragIcon;

		OutOperation = DragDropOp;
	}
}
bool UARAbilityWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARAbilityWidget* Payload = Cast<UARAbilityWidget>(InOperation->Payload);
	Setbility(Payload->AbilityTag);

	OwningComponent->NativeEquipAbility(Payload->AbilityTag, AbilitySetIndex, AbilityIndex);
	return false;
}