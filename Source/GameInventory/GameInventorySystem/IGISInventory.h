#pragma once
#include "GISGlobalTypes.h"
#include "IGISInventory.generated.h"



UINTERFACE()
class GAMEINVENTORYSYSTEM_API UIGISInventory : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGISInventory
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual class UGISInventoryBaseComponent* GetInventory() = 0;
};