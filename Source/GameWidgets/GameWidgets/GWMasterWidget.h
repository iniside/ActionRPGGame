#pragma once
#include "Blueprint/UserWidget.h"
#include "GWMasterWidget.generated.h"
/*
	Master widget, doesn't implement any special functionality.
	It exist only as base for blueprint widget, which takes entire screen, 
	and can hold other widgets.
*/
UCLASS()
class GAMEWIDGETS_API UGWMasterWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGWMasterWidget(const FObjectInitializer& ObjectInitializer);
};
