#pragma once
#include "GASTrace.h"
#include "GTGlobalTypes.h"
#include "GASTrace_SphereTrace.generated.h"
/*
	This actually could be moved into separate module ? I know i might need something
	very similiar inside Effectsm but on the other hand the Within part makes
	it very easy to use in first place.

	Draft Plan:
	1. Move classes to separate module.
	2. Make them dependand on single object like UTraceObject
	3. If any other object or actor want to use these tracing object, they would need to 
	have instance of UTraceObject.
	4. Another possibility is to make, Interafce, which would be implemented by object/actor, which
	want to make use of this module.


	5. Or I can just as well copy/paste these files to different modules. Not that good, since well
	to much updateding!.

	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASTrace_SphereTrace : public UGASTrace
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AGTTraceDisplayHelper> TargetingHelperClass;
	
	UPROPERTY(EditAnywhere)
		TArray<TEnumAsByte<ECollisionChannel> > SphereObjectToTrace;

	FCollisionObjectQueryParams SphereCollisionObjectParams;

	UPROPERTY()
	class AGTTraceDisplayHelper* TargetingHelper;

	virtual void Tick(float DeltaTime) override;
	virtual void Initialize() override;
	virtual void PreExecute() override;
	virtual void Execute() override;
	virtual void PostExecute() override;
	void SphereTrace();
};
