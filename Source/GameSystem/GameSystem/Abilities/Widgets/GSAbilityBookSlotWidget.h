#pragma once
#include "UserWidget.h"
#include "GSAbilityBookSlotWidget.generated.h"

/*
	It's not ability book, per se, but it contains non instanced abilities (classes).

	Use it as base to create real ability book, skill selection etc.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityBookSlotWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
		FGSAbilityBookSlot SlotInfo;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UGSAbilityBookItem* AbilityItem;

	UPROPERTY()
		FName SlotName;
public:
	FEventReply OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;

	void OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation) override;

	bool OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation) override;
};
