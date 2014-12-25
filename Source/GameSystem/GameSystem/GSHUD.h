#pragma once
#include "Widgets/FCTHudWidget.h"
#include "GSHUD.generated.h"
/*
	Not deicded yet how exactly will this work.
	The plan is to have single widget which will manage incoming values.
	And then you inherit from it in blueprint to setup animations/how exactly values will be rendered.
*/
UCLASS()
class GAMESYSTEM_API AGSHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Floating Combat Text")
		UFCTHudWidget* FCTWidget;


	virtual void DrawHUD() override;
};
