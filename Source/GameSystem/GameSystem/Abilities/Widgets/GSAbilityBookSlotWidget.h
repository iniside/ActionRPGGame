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
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
