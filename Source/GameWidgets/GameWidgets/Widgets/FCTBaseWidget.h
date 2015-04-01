#pragma once
#include "Blueprint/UserWidget.h"
#include "FCTGlobalTypes.h"
#include "FCTBaseWidget.generated.h"
/*
	Not deicded yet how exactly will this work.
	The plan is to have single widget which will manage incoming values.
	And then you inherit from it in blueprint to setup animations/how exactly values will be rendered.
*/
UCLASS()
class GAMEWIDGETS_API UFCTBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
		class UFCTFloatingTextComponent* OwningComponent;

	UPROPERTY()
		TSubclassOf<class UFCTFloatingWidget> FloatingTextType;

	UPROPERTY()
		TWeakObjectPtr<APlayerController> PCOwner;
protected:

public:
	float FloatingTextLifeTime;

	int32 MaximumIndicators;

	void InitializeWidget();

	//virtual void Tick_Implementation(FGeometry MyGeometry, float InDeltaTime) override;

	void TickMe(float DeltaTimeIn);

	void RecivedData(const FFCTDisplayData& DataIn);
};
