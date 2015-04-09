#pragma once
#include "UserWidget.h"
#include "GSEquipmentSlotWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSEquipmentSlotWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	FEventReply OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;

	void OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation) override;

	bool OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation) override;
};
