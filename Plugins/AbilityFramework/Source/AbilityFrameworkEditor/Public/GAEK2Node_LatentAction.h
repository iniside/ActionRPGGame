
#pragma once
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_BaseAsyncTask.h"
#include "GAEK2Node_LatentAction.generated.h"

UCLASS()
class UGAEK2Node_LatentAction : public UK2Node_BaseAsyncTask
{
	GENERATED_BODY()

public:
	UGAEK2Node_LatentAction(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
};
