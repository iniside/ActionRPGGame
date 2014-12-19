#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributesBlueprintFunctionLibrary.generated.h"
/*
	This is base class for item container used in InventoryComponent, to store items.

	How you use this container is up to you. I assume that item container will contain only TSubclassOf<>
	properties, which will in turn reference proper items (like weapons, construction elements, 
	consumable, armor items etc), along with any accompaning data for them (like upgrades, stats mods,
	count of how many items player have etc).

	If you so desire, you can of course just extend any item from this class but I do not recommend it.
*/
UCLASS()
class GAMEATTRIBUTES_API UGAAttributesBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
	static FName GetAttribute(FGAAttribute AttributeIn);
};
