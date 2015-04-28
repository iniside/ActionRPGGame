#pragma once
#include "GWMasterWidget.h"
#include "GSMasterWidget.generated.h"
/*
	Master widget, doesn't implement any special functionality.
	It exist only as base for blueprint widget, which takes entire screen, 
	and can hold other widgets.
*/
UCLASS()
class GAMESYSTEM_API UGSMasterWidget : public UGWMasterWidget
{
	GENERATED_BODY()
public:
	UGSMasterWidget(const FObjectInitializer& ObjectInitializer);

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
