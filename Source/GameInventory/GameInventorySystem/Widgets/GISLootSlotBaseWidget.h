#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISLootSlotBaseWidget.generated.h"
/*
	Base Widget for Tabs. Each Tab contain, slot widgets. How Tab is going to display it's widgets it's up to
	blueprint extendting from this class.
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISLootSlotBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	~UGISLootSlotBaseWidget();
	UPROPERTY()
		FGISLootSlotInfo LootSlotInfo;

	UPROPERTY()
	class UGISItemBaseWidget* ItemWidget;

	void SetItemInfo(int32 ItemIndex);

	void ResetSlot();

	virtual FEventReply OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;

	void OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation) override;

	bool OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation) override;
};
