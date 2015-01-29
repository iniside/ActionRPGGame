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

	FEventReply OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;

	void OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation) override;

	bool OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation) override;
};
