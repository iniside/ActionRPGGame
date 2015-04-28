#pragma once

#include "GISBlueprintFunctionLibrary.generated.h"
/*
	This is base class for item container used in InventoryComponent, to store items.

	How you use this container is up to you. I assume that item container will contain only TSubclassOf<>
	properties, which will in turn reference proper items (like weapons, construction elements, 
	consumable, armor items etc), along with any accompaning data for them (like upgrades, stats mods,
	count of how many items player have etc).

	If you so desire, you can of course just extend any item from this class but I do not recommend it.
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Create Item Data", BlueprintInternalUseOnly = "true"), Category = "Game Inventory System")
		static class UGISItemData* CreateItem(UObject* WorldContextObject, TSubclassOf<class UGISItemData> ItemClass, APlayerController* OwningPlayer);
};
