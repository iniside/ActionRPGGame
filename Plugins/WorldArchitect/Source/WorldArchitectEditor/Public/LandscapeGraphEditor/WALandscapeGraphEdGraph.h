#pragma once

#include "WALandscapeGraphEdGraph.generated.h"

UCLASS()
class UWALandscapeGraphEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UWALandscapeGraphEdGraph();
	virtual ~UWALandscapeGraphEdGraph();

	virtual void RebuildGenericGraph();

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
};
