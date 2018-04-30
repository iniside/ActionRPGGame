#pragma once
#include "AFTaskBase.h"
#include "GAWaitAction.generated.h"
/*
	AbilityActions are generic (preferably C++) defined actions, which then can be added to ability and
	the should be activated from ability. 
	Then can perform tasks, like spawn tagetting helpers (splines, circles), spawn actors, 
	gather targeting data etc.

	Should they be activated automatically after ability is initialized, (it'e ability enterted in
	active state, which means it's ready to be fired and display helpers, but did not yet received input,
	or should designer in blueprint decide when to launch actions ?).
*/
UCLASS(meta = (ExposedAsyncProxy = "true") )
class ABILITYFRAMEWORK_API UGAWaitAction : public UAFTaskBase
{
	GENERATED_BODY()

public:
	UGAWaitAction(const FObjectInitializer& ObjectInitializer);
	float Time;
	float TimeStarted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAWaitActionDelegate);
	UPROPERTY(BlueprintAssignable)
		FGAWaitActionDelegate OnInitialized;
	UPROPERTY(BlueprintAssignable)
		FGAWaitActionDelegate OnFinish;

	UPROPERTY(BlueprintAssignable)
		FGAWaitActionDelegate OnTick;

	//virtual void Tick(float DeltaSecondsIn);

	virtual void Activate() override;
	virtual void TickTask(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Latent Actions", meta = (AdvancedDisplay = "InTaskOwner, Priority", DefaultToSelf = "InTaskOwner", BlueprintInternalUseOnly = "TRUE"))
		static UGAWaitAction* NewGAWaitAction(UObject* InTaskOwner, float Time);

	void OnTimeFinish();

};
