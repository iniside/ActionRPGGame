#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISTabBaseWidget.generated.h"
/*
	Base Widget for Tabs. Each Tab contain, slot widgets. How Tab is going to display it's widgets it's up to
	blueprint extendting from this class.
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISTabBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<class UGISSlotBaseWidget*> InventorySlots;

	UPROPERTY(BlueprintReadOnly)
		FGISTabInfo TabInfo;

	int32 ItemCount;
};
