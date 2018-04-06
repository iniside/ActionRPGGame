
#pragma once
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_BaseAsyncTask.h"
#include "GAEK2Node_LatentAbilityTaskCall.generated.h"

UCLASS()
class UGAEK2Node_LatentAbilityTaskCall : public UK2Node_BaseAsyncTask//UK2Node_LatentGameplayTaskCall
{
	GENERATED_BODY()

public:
	UGAEK2Node_LatentAbilityTaskCall(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
	// End of UEdGraphNode interface
};
