#pragma once

#include "WALandscapeNode.h"
#include "WALandscapeGraph.generated.h"

UCLASS(Blueprintable)
class WORLDARCHITECTEDITOR_API UWALandscapeGraph : public UObject
{
	GENERATED_BODY()

public:
	UWALandscapeGraph();
	virtual ~UWALandscapeGraph();

	//////////////////////////////////////////////////////////////////////////
	// uproperties
	UPROPERTY(EditAnywhere, Category = "GenericGraph")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "GenericGraph")
	TSubclassOf<UWALandscapeNode> NodeType;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraph")
		UWALandscapeNode* LandscapeOutput;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraph")
	TArray<UWALandscapeNode*> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraph")
	TArray<UWALandscapeNode*> AllNodes;

//#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* EdGraph;

	UPROPERTY()
	class UWALandscapeGraphSchema* Schema;
//#endif
	void GenerateLandscape();
	void ClearGraph();
};
