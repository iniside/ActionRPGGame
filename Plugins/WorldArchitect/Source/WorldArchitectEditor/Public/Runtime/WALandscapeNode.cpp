#include "WALandscapeNode.h"
#include "WALandscapeGraph.h"
#define LOCTEXT_NAMESPACE "GenericGraphNode"

UWALandscapeNode::UWALandscapeNode()
{
	BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

UWALandscapeNode::~UWALandscapeNode()
{

}

FString UWALandscapeNode::GetNodeTitle()
{
	return CustomNodeTitle;
}

UWALandscapeGraph* UWALandscapeNode::GetGraph()
{
	return Cast<UWALandscapeGraph>(GetOuter());
}

TArray<uint16> UWALandscapeNode::GenerateHeightmap()
{
	TArray<uint16> RetVal;
	return RetVal;
}

#undef LOCTEXT_NAMESPACE
