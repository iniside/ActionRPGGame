// Fill out your copyright notice in the Description page of Project Settings.
#include "WALandscapeGraphEdNode_Multiply.h"
#include "WALandscapeGraphEditorTypes.h"
#include "../WALandscapeNode_Multiply.h"


void UWALandscapeGraphEdNode_Multiply::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UWALandscapeGraphEditorTypes::PinCategory_InputA, TEXT("In A"));
	CreatePin(EGPD_Input, UWALandscapeGraphEditorTypes::PinCategory_InputB, TEXT("In B"));
	CreatePin(EGPD_Output, UWALandscapeGraphEditorTypes::PinCategory_Output, TEXT("Out"));
}

void UWALandscapeGraphEdNode_Multiply::PinConnectionListChanged(UEdGraphPin* Pin)
{
	//UWALandscapeGraphEdNode_Output* InputNode = nullptr;
	//UWALandscapeGraphEdNode_Multiply* ThisEdNode = this;
	//UWALandscapeNode_Multiply* ThisNode = Cast<UWALandscapeNode_Multiply>(ThisEdNode->GenericGraphNode);
	UWALandscapeGraphEdNode* ThisEdNode = Cast<UWALandscapeGraphEdNode>(Pin->GetOwningNode());
	UWALandscapeNode_Multiply* ThisNode = Cast<UWALandscapeNode_Multiply>(ThisEdNode->GenericGraphNode);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
	{
		if (Pin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_InputA)
		{
			//InputNode = Cast<UWALandscapeGraphEdNode_Output>(Pin->GetOwningNode());
			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				if (LinkedPin->Direction == EEdGraphPinDirection::EGPD_Output
					&& LinkedPin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_Output)
				{
					UWALandscapeGraphEdNode* InputNode = Cast<UWALandscapeGraphEdNode>(LinkedPin->GetOwningNode());
					InputA = InputNode;
					if (ThisEdNode != InputNode)
					{
						UWALandscapeNode* NodeA = InputNode->GenericGraphNode;
						ThisNode->InputA = NodeA;
					}
				}
			}
		}
		else if (Pin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_InputB)
		{
			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				if (LinkedPin->Direction == EEdGraphPinDirection::EGPD_Output
					&& LinkedPin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_Output)
				{
					UWALandscapeGraphEdNode* InputNode = Cast<UWALandscapeGraphEdNode>(LinkedPin->GetOwningNode());

					if (ThisEdNode != InputNode)
					{
						InputB = InputNode;
						UWALandscapeNode* NodeB = InputNode->GenericGraphNode;
						ThisNode->InputB = NodeB;
					}
				}
			}
		}
	}
	else if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		//InputNode = Cast<UWALandscapeGraphEdNode>(Pin->GetOwningNode());
	}
}
void UWALandscapeGraphEdNode_Multiply::GenerateHeightMap(TArray<uint16>& InOutMap)
{
	TArray<uint16> OutputA;
	TArray<uint16> OutputB;

	if (InputA)
	{
		InputA->GenerateHeightMap(OutputA);
	}
	if (InputB)
	{
		InputB->GenerateHeightMap(OutputB);
	}
}
