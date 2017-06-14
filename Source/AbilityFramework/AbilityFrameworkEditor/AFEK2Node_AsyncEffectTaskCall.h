
#pragma once
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_LatentGameplayTaskCall.h"
#include "AFEK2Node_AsyncEffectTaskCall.generated.h"

UCLASS()
class UAFEK2Node_AsyncEffectTaskCall : public UK2Node_LatentGameplayTaskCall
{
	GENERATED_BODY()

public:
	UAFEK2Node_AsyncEffectTaskCall(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
	// End of UEdGraphNode interface
protected:
	virtual bool IsHandling(TSubclassOf<UGameplayTask> TaskClass) const override;
};
