#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISLootContainerBaseWidget.generated.h"
/*
	Base Widget for Tabs. Each Tab contain, slot widgets. How Tab is going to display it's widgets it's up to
	blueprint extendting from this class.
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISLootContainerBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, Instanced)
		TSubclassOf<class UGISLootSlotBaseWidget> SlotClass;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UGISLootSlotBaseWidget*> Slots;

	UPROPERTY(BlueprintReadOnly)
		TArray<FGISLootSlotInfo> ItemsInfos;

	UPROPERTY()
		class AGISPickupActor* OwningPickupActor;

	void InitializeLootWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void PostLootWidgetInitialized();
};
