
#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_LatentGameplayTaskCall.h"
#include "GAEK2Node_LatentAbilityTaskCall.generated.h"

UCLASS()
class UGAEK2Node_LatentAbilityTaskCall : public UK2Node_LatentGameplayTaskCall
{
	GENERATED_BODY()

public:
	UGAEK2Node_LatentAbilityTaskCall(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* TargetGraph) const override;
	// End of UEdGraphNode interface

	//virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
protected:
	virtual bool IsHandling(TSubclassOf<UGameplayTask> TaskClass) const override;

	//UEdGraphPin* GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch = nullptr) const;
	//UClass* GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch = nullptr) const;
};
