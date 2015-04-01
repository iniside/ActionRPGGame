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
	GENERATED_BODY()
protected:
	UPROPERTY()
		FGISLootConfiguration Config;

	UPROPERTY()
		class AGISPickupActor* OwningPickupActor;

	UPROPERTY()
	class UGISInventoryBaseComponent* OwningComp;

	UPROPERTY()
		APlayerController* PCOwner;

	UPROPERTY()
		TArray<class UGISLootSlotBaseWidget*> Slots;
public:
	UGISLootContainerBaseWidget(const FObjectInitializer& ObjectInitializer);

	void InitializeLootWidget(const FGISLootConfiguration& ConfigIn, class UGISInventoryBaseComponent* OwningCompIn,
		APlayerController* PCOwnerIn);

	UFUNCTION()
	void UpdateLootWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void PostLootWidgetInitialized();
};
