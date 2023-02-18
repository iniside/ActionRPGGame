#include "WALandscapeGraph.h"

#define LOCTEXT_NAMESPACE "GenericGraph"

UWALandscapeGraph::UWALandscapeGraph()
{
	NodeType = UWALandscapeNode::StaticClass();

#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;
#endif
}

UWALandscapeGraph::~UWALandscapeGraph()
{

}

void UWALandscapeGraph::ClearGraph()
{
}
void UWALandscapeGraph::GenerateLandscape()
{

}
#undef LOCTEXT_NAMESPACE
