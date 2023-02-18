#include "WALandscapeGraphEdNode.h"
#include "WALandscapeGraphEdGraph.h"
#include "WALandscapeGraph.h"
#include "WALandscapeNode.h"
#include "WALandscapeGraphEditorTypes.h"
#include "EdGraph/EdGraphPin.h"
#define LOCTEXT_NAMESPACE "GenericGraphEdNode"

void UWALandscapeGraphEdNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UWALandscapeGraphEditorTypes::PinCategory_SingleNode, TEXT("Height Out"));
	CreatePin(EGPD_Output, UWALandscapeGraphEditorTypes::PinCategory_MaskInput, TEXT("Height In"));
	//CreatePin(EGPD_Output, UWALandscapeGraphEditorTypes::PinCategory_SingleNode, TEXT(""), NULL, false, false, TEXT("Height Out"));
	//CreatePin(EGPD_Input, UWALandscapeGraphEditorTypes::PinCategory_MaskInput, TEXT(""), NULL, false, false, TEXT("Mask In"));
}

void UWALandscapeGraphEdNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();

	GetGenericGraphEdGraph()->RebuildGenericGraph();
}

UWALandscapeGraphEdGraph* UWALandscapeGraphEdNode::GetGenericGraphEdGraph()
{
	return Cast<UWALandscapeGraphEdGraph>(GetGraph());
}

FText UWALandscapeGraphEdNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (GenericGraphNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return FText::FromString(GenericGraphNode->GetNodeTitle());
	}
}

void UWALandscapeGraphEdNode::SetGenericGraphNode(UWALandscapeNode* InNode)
{
	GenericGraphNode = InNode;
}

FText UWALandscapeGraphEdNode::GetDescription() const
{
	UWALandscapeGraph* Graph = GenericGraphNode->GetGraph();

	return FText::FromString("Dupa Description");
}

FLinearColor UWALandscapeGraphEdNode::GetBackgroundColor() const
{
	return GenericGraphNode->BackgroundColor;
}
void UWALandscapeGraphEdNode::PinConnectionListChanged(UEdGraphPin* Pin)
{

}
void UWALandscapeGraphEdNode::GenerateHeightMap(TArray<uint16>& InOutMap)
{

}
#undef LOCTEXT_NAMESPACE
