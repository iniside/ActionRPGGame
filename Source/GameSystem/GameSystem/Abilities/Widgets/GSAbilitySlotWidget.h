#pragma once
#include "UserWidget.h"
#include "../GSAbilityGlobalTypes.h"
#include "GSAbilitySlotWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilitySlotWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
		UGSAbilityWidget* Ability;
	UPROPERTY()
		FGSAbilitySlot SlotInfo;
	UPROPERTY()
	class UGSAbilitiesComponent* OwnerComp;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
