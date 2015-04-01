#pragma once
#include "FCTGlobalTypes.h"
#include "FCTFloatingTextComponent.generated.h"

/*
	Called when new information to display has been recived;
*/


/*
	Possibly make it UObject ? Don't need replication, it's purerly client side.
*/
/*
	Component will house widgets, which will display any data over actor, to which this component is 
	attached.
*/
UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEWIDGETS_API UFCTFloatingTextComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Amount of indicators which this component can display at once.
		It's per attached component setting!. Generally keep it low.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaximumIndicators;

	UPROPERTY(EditAnywhere, Category = "Config")
		float FloatingTextLifeTime;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVector TextOffset;

	FFCTOnReceivedData OnReceivedData;

	UPROPERTY(EditAnywhere, Category = "Config")
		TSubclassOf<class UFCTFloatingWidget> FloatingTextType;

	UPROPERTY(BlueprintReadOnly)
		TArray<FFCTDisplayData> CachedDisplayData;

	UPROPERTY()
		APlayerController* PCOwner;

	UPROPERTY(BlueprintReadOnly)
		TArray<UFCTFloatingWidget*> FloatingWidgets;

	void RecivedData(const FFCTDisplayData& DataIn);
public:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};



