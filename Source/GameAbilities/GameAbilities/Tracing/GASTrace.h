#pragma once
#include "GTTraceBase.h"
#include "GTGlobalTypes.h"
#include "GASTrace.generated.h"
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
class GAMEABILITIES_API UGASTrace : public UGTTraceBase
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Name of socket, which you will want to use for spawning effects.
		It might be different from the one used for tracing hit location.
	 */
	UPROPERTY(EditAnywhere)
	FName EffectOriginLocation;
protected:
	FVector TraceArea;
public:
	virtual void Tick(float DeltaSecondsIn) override;
	inline void SetTraceArea(FVector TraceAreaIn){ TraceArea = TraceAreaIn; };
	void SingleLineTraceSetHitLocation();
};
