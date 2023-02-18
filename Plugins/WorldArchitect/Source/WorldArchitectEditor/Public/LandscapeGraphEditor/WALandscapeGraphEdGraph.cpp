#include "WALandscapeGraphEdGraph.h"
#include "WALandscapeGraphEdNode.h"
#include "WALandscapeGraph.h"
#include "WALandscapeGraphEdGraph.h"
#include "WALandscapeGraphEditorTypes.h"
#include "EdGraph/EdGraphPin.h"

UWALandscapeGraphEdGraph::UWALandscapeGraphEdGraph()
{

}

UWALandscapeGraphEdGraph::~UWALandscapeGraphEdGraph()
{

}

void UWALandscapeGraphEdGraph::RebuildGenericGraph()
{
	//UWALandscapeGraph* G = CastChecked<UWALandscapeGraph>(GetOuter());

	//G->ClearGraph();
	//for (int i = 0; i < Nodes.Num(); ++i)
	//{
	//	UWALandscapeGraphEdNode* EdNode = Cast<UWALandscapeGraphEdNode>(Nodes[i]);

	//	if (EdNode == nullptr || EdNode->GenericGraphNode == nullptr)
	//		continue;

	//	UWALandscapeNode* GNode = EdNode->GenericGraphNode;

	//	G->AllNodes.Add(GNode);
	//	GNode->MaskNode = nullptr;
	//	for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
	//	{
	//		UEdGraphPin* Pin = EdNode->Pins[PinIdx];

	//		if (Pin->Direction == EEdGraphPinDirection::EGPD_Input 
	//			&& Pin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_MaskInput)
	//		{
	//			UWALandscapeGraphEdNode* InputNode = nullptr;
	//			InputNode = Cast<UWALandscapeGraphEdNode>(Pin->GetOwningNode());
	//			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
	//			{
	//				if (LinkedPin->Direction == EEdGraphPinDirection::EGPD_Output
	//					&& LinkedPin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_SingleNode)
	//				{
	//					UWALandscapeGraphEdNode* OutputNode = Cast<UWALandscapeGraphEdNode>(LinkedPin->GetOwningNode());

	//					if (OutputNode != InputNode)
	//					{
	//						UWALandscapeNode* nodeInput = InputNode->GenericGraphNode;
	//						UWALandscapeNode* nodeOutput = OutputNode->GenericGraphNode;
	//						if (nodeInput && nodeOutput)
	//						{
	//							nodeInput->MaskNode = nodeOutput;
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

#if WITH_EDITOR
void UWALandscapeGraphEdGraph::PostEditUndo()
{
	Super::PostEditUndo();

	RebuildGenericGraph();

	Modify();
}
#endif

