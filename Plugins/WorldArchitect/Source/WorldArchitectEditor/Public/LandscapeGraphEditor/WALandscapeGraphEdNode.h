#pragma once
#include "EdGraph/EdGraphNode.h"
#include "WALandscapeGraphEdNode.generated.h"

UCLASS(MinimalAPI)
class UWALandscapeGraphEdNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
		UWALandscapeGraphEdNode* Output;

	UPROPERTY(VisibleAnywhere, instanced, Category = "GenericGraph")
	class UWALandscapeNode* GenericGraphNode;

	virtual void AllocateDefaultPins() override;
	virtual void NodeConnectionListChanged() override;

	class UWALandscapeGraphEdGraph* GetGenericGraphEdGraph();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const;

	void SetGenericGraphNode(UWALandscapeNode* InNode);

	virtual FText GetDescription() const;

	FLinearColor GetBackgroundColor() const;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void GenerateHeightMap(TArray<uint16>& InOutMap);

};
