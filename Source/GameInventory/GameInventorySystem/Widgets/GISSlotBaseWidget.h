#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISSlotBaseWidget.generated.h"

UCLASS()
class GAMEINVENTORYSYSTEM_API UGISSlotBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FGISSlotInfo SlotInfo;

	UPROPERTY(BlueprintReadOnly)
		FGISTabInfo TabInfo;

	UPROPERTY()
		FName DropSlottName;

	UPROPERTY()
	class UGISItemBaseWidget* ItemInSlot;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
