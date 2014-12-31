#pragma once
#include "GameTrace.h"
#include "GTTraceBase.h"
#include "GTTrace_LineMulti.generated.h"
/*
	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew)
class GAMETRACE_API UGTTrace_LineMulti : public UGTTraceBase
{
	GENERATED_UCLASS_BODY()
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

};
