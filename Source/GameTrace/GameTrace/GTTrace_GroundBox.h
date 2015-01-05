#pragma once
#include "GameTrace.h"
#include "GTTraceBase.h"
#include "GTTrace_GroundBox.generated.h"
/*
	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew)
class GAMETRACE_API UGTTrace_GroundBox : public UGTTraceBase
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Configuration")
		TSubclassOf<class AGTTraceDisplayHelper> DisplayHelperActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
		FVector BoxSize;

	UPROPERTY(EditAnywhere, Category = "Configuration")
		TArray<TEnumAsByte<ECollisionChannel> > BoxObjectsToTrace;

	virtual FVector GetHelperScale() override;

	/**/
	virtual void Tick(float DeltaSecondsIn) override;

	virtual void Initialize() override;

	/**
	*	Call it to display cosmetic helpers.
	*/
	virtual void PreExecute() override;

	/**
	 *	Central function to execute current action.
	 */
	virtual void Execute() override;

	virtual void PostExecute() override;
protected:
	UPROPERTY()
	class AGTTraceDisplayHelper* DisplayHelper;

	FCollisionObjectQueryParams BoxObjectParams;

	void TraceBox();
};
