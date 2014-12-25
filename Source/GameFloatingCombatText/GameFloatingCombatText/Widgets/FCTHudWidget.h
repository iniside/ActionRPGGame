#pragma once
#include "FCTGlobalTypes.h"
#include "FCTHudWidget.generated.h"
/*
	Not deicded yet how exactly will this work.
	The plan is to have single widget which will manage incoming values.
	And then you inherit from it in blueprint to setup animations/how exactly values will be rendered.

	Make it actor component to be attachable to AHUD in blueprint ?
*/
UCLASS()
class GAMEFLOATINGCOMBATTEXT_API UFCTHudWidget : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere)
		FFCTDisplaySettings FCTSettings;

	UPROPERTY()
		APawn* HUDPawn;

	FFCTOnReceivedData OnReceivedData;

	TArray<FFCTDamageHudIndicator> DamageIndicators;

	void PawnDamaged(const FFCTDisplayData& UIDamage);

	void DrawDamageIndicators(UCanvas* CanvasIn, float DeltaTime);
};
