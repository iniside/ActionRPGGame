#pragma once
#include "UserWidget.h"
#include "../GSWeaponEquipmentComponent.h"
#include "GSWeaponSetSlotWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSWeaponSetSlotWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FGSWeaponSetSlot WeaponSlot;

	FName DropSlottName;

	UPROPERTY()
		UGSWeaponSetItemWidget* ItemWidget;

	UPROPERTY()
	class UGSWeaponEquipmentComponent* WeaponEquipComp;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
