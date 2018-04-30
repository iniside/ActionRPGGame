// Fill out your copyright notice in the Description page of Project Settings.
#include "WALandscapeGraphEdNode_Output.h"
#include "WALandscapeGraphEditorTypes.h"
#include "WALandscapeNode_Output.h"



void UWALandscapeGraphEdNode_Output::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UWALandscapeGraphEditorTypes::PinCategory_FinalInput, TEXT("In"));
}

void UWALandscapeGraphEdNode_Output::PinConnectionListChanged(UEdGraphPin* Pin)
{
	UWALandscapeGraphEdNode_Output* InputNode = nullptr;
	UWALandscapeGraphEdNode_Output* NodeToAdd = this;
	if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
	{
		if (Pin->PinType.PinCategory == UWALandscapeGraphEditorTypes::PinCategory_FinalInput)
		{
			InputNode = Cast<UWALandscapeGraphEdNode_Output>(Pin->GetOwningNode());
			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				if (LinkedPin->Direction == EEdGraphPinDirection::EGPD_Output)
				{
					UWALandscapeGraphEdNode* OutputNode = Cast<UWALandscapeGraphEdNode>(LinkedPin->GetOwningNode());

					Input = OutputNode;
					if (OutputNode != InputNode)
					{
						UWALandscapeNode_Output* nodeInput = Cast<UWALandscapeNode_Output>(InputNode->GenericGraphNode);
						UWALandscapeNode* nodeOutput = OutputNode->GenericGraphNode;
						if (nodeInput && nodeOutput)
						{
							
							nodeInput->Heightmap = nodeOutput;
						}
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
void UWALandscapeGraphEdNode_Output::GenerateHeightMap(TArray<uint16>& InOutMap)
{
	if (Input)
	{
		TArray<uint16> Out;
		Input->GenerateHeightMap(Out);
	}
}
