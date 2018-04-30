#include "WALandscapeGraphSchema.h"
#include "WALandscapeGraphEdNode.h"
#include "WALandscapeGraphConnectionDrawingPolicy.h"
#include "WALandscapeGraph.h"
#include "WALandscapeNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "WALandscapeGraphEdNode_Output.h"
#include "WALandscapeNode_Output.h"
#include "WALandscapeGraphEdNode_Multiply.h"
#include "../WALandscapeNode_Multiply.h"
#include "../WALGEdNode_Start.h"
#include "GenericCommands.h"

#define LOCTEXT_NAMESPACE "GenericGraphAssetSchema"

int32 UWALandscapeGraphSchema::CurrentCacheRefreshID = 0;

class FNodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{
		VisitedNodes.Add(StartNode);

		return TraverseInputNodesToRoot(EndNode);
	}

private:
	bool TraverseInputNodesToRoot(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* MyPin = Node->Pins[PinIndex];

			if (MyPin->Direction == EGPD_Output)
			{
				for (int32 LinkedPinIndex = 0; LinkedPinIndex < MyPin->LinkedTo.Num(); ++LinkedPinIndex)
				{
					UEdGraphPin* OtherPin = MyPin->LinkedTo[LinkedPinIndex];
					if (OtherPin)
					{
						UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
						if (VisitedNodes.Contains(OtherNode))
						{
							return false;
						}
						else
						{
							return TraverseInputNodesToRoot(OtherNode);
						}
					}
				}
			}
		}

		return true;
	}

	TSet<UEdGraphNode*> VisitedNodes;
};

UEdGraphNode* FWALandscapeGraphAssetSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UWALandscapeGraph* Graph = CastChecked<UWALandscapeGraph>(ParentGraph->GetOuter());

	//const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorNewNode", "Generic Graph Editor: New Node"));
	ParentGraph->Modify();
	Graph->Modify();

	UWALandscapeNode* NewNode = NewObject<UWALandscapeNode>(Graph, NodeType);

	//Graph->AllNodes.Add(NewNode);

	//FGraphNodeCreator<UWALandscapeGraphEdNode> NodeCreator(*Graph->EdGraph);
	//UWALandscapeGraphEdNode* GraphNode = NodeCreator.CreateNode(true);
	//GraphNode->SetGenericGraphNode(NewNode);
	//NodeCreator.Finalize();

	//GraphNode->NodePosX = Location.X;
	//GraphNode->NodePosY = Location.Y;

	//GraphNode->AutowireNewNode(FromPin);

	//Graph->PostEditChange();
	//Graph->MarkPackageDirty();

	//return GraphNode;

	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();

		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		//int32 XLocation = Location.X;
		//if (FromPin && FromPin->Direction == EGPD_Input)
		//{
		//	UEdGraphNode* PinNode = FromPin->GetOwningNode();
		//	const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

		//	if (XDelta < NodeDistance)
		//	{
		//		// Set location to edge of current node minus the max move distance
		//		// to force node to push off from connect node enough to give selection handle
		//		XLocation = PinNode->NodePosX - NodeDistance;
		//	}
		//}

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		// setup pins after placing node in correct spot, since pin sorting will happen as soon as link connection change occurs
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);
		NodeTemplate->SetGenericGraphNode(NewNode);
		Graph->PostEditChange();
		Graph->MarkPackageDirty();

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}
void UWALandscapeGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	{
		FGraphNodeCreator<UWALandscapeGraphEdNode_Output> NodeCreator(Graph);
		UWALandscapeGraph* LandscapeGraph = CastChecked<UWALandscapeGraph>(Graph.GetOuter());
		UWALandscapeGraphEdNode_Output* MyNode = NodeCreator.CreateNode();
		Output = MyNode;
		UWALandscapeNode_Output* NewNode = NewObject<UWALandscapeNode_Output>(LandscapeGraph, UWALandscapeNode_Output::StaticClass());
		LandscapeGraph->LandscapeOutput = NewNode;
		MyNode->SetGenericGraphNode(NewNode);
		NodeCreator.Finalize();
		SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
	}
	//{
	//	FGraphNodeCreator<UWALGEdNode_Start> NodeCreator(Graph);
	//	UWALandscapeGraph* LandscapeGraph = CastChecked<UWALandscapeGraph>(Graph.GetOuter());
	//	UWALGEdNode_Start* MyNode = NodeCreator.CreateNode();
	//	//UWALandscapeNode_Output* NewNode = NewObject<UWALandscapeNode_Output>(LandscapeGraph, UWALandscapeNode_Output::StaticClass());
	//	//LandscapeGraph->LandscapeOutput = NewNode;
	//	//MyNode->SetGenericGraphNode(NewNode);
	//	NodeCreator.Finalize();
	//	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
	//}
	//WALGEdNode_Start
}
void UWALandscapeGraphSchema::GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

	// Add all the links we could break from
	for (TArray<class UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			//TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName);

			// Add name of connection if possible
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32 &Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		++Count;

		//MenuBuilder.AddMenuEntry(Description, Description, FSlateIcon(), FUIAction(
		//	FExecuteAction::CreateUObject(this, &UWALandscapeGraphSchema::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

void UWALandscapeGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	{
		const FText AddToolTip;
		const FText Desc = LOCTEXT("NewGenericGraphNodeTooltip", "Multiply");
		TSharedPtr<FWALandscapeGraphAssetSchemaAction_NewNode> NewNodeAction(new FWALandscapeGraphAssetSchemaAction_NewNode(LOCTEXT("GenericGraphNodeAction", "Common"), Desc, AddToolTip, 1));
		NewNodeAction->NodeTemplate = NewObject<UWALandscapeGraphEdNode_Multiply>(ContextMenuBuilder.OwnerOfTemporaries);
		NewNodeAction->NodeType = UWALandscapeNode_Multiply::StaticClass();
		ContextMenuBuilder.AddAction(NewNodeAction);
	}
}

void UWALandscapeGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if (InGraphPin != nullptr)
	{
		MenuBuilder->BeginSection("GenericGraphAssetGraphSchemaNodeActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		{
			// Only display the 'Break Link' option if there is a link to break!
			if (InGraphPin->LinkedTo.Num() > 0)
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);

				// add sub menu for break link to
				if (InGraphPin->LinkedTo.Num() > 1)
				{
					MenuBuilder->AddSubMenu(
						LOCTEXT("BreakLinkTo", "Break Link To..."),
						LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
						FNewMenuDelegate::CreateUObject((UWALandscapeGraphSchema*const)this, &UWALandscapeGraphSchema::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));
				}
				else
				{
					((UWALandscapeGraphSchema*const)this)->GetBreakLinkToSubMenuActions(*MenuBuilder, const_cast<UEdGraphPin*>(InGraphPin));
				}
			}
		}
		MenuBuilder->EndSection();
	}
	else if(InGraphNode != nullptr)
	{
		MenuBuilder->BeginSection("GenericGraphAssetGraphSchemaNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		{
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);

			MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
		MenuBuilder->EndSection();
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

const FPinConnectionResponse UWALandscapeGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}

	// Compare the directions
	if ((A->Direction == EGPD_Input) && (B->Direction == EGPD_Input))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorInput", "Can't connect input node to input node"));
	}
	else if ((A->Direction == EGPD_Output) && (B->Direction == EGPD_Output))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorOutput", "Can't connect output node to output node"));
	}

	// check for cycles
	FNodeVisitorCycleChecker CycleChecker;
	if (!CycleChecker.CheckForLoop(A->GetOwningNode(), B->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
}

class FConnectionDrawingPolicy* UWALandscapeGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FWALandscapeGraphConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UWALandscapeGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UWALandscapeGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UWALandscapeGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

//void UWALandscapeGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin)
//{
//	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));
//
//	Super::BreakSinglePinLink(SourcePin, TargetPin);
//}

bool UWALandscapeGraphSchema::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UWALandscapeGraphSchema::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UWALandscapeGraphSchema::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}
#undef LOCTEXT_NAMESPACE
