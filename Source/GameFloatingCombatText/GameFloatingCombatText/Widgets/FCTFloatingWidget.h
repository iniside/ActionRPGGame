#pragma once
#include "Blueprint/UserWidget.h"
#include "FCTGlobalTypes.h"
#include "FCTFloatingWidget.generated.h"
/*
	Not deicded yet how exactly will this work.
	The plan is to have single widget which will manage incoming values.
	And then you inherit from it in blueprint to setup animations/how exactly values will be rendered.
*/
UCLASS()
class GAMEFLOATINGCOMBATTEXT_API UFCTFloatingWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FFCTDisplayData DisplayData;

	UPROPERTY()
		TWeakObjectPtr<APlayerController> PCOwner;

	float LifeTime;


	//virtual void Construct_Implementation() override;
//	virtual void Tick_Implementation(FGeometry MyGeometry, float InDeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Floating Combat Text")
	void MyTick(float DeltaTimeIn);
private:
	
};
