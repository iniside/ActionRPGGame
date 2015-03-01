
#pragma once

#include "GAGlobalTypesEditor.generated.h"

USTRUCT()
struct FGAAttributeNode
{
	GENERATED_USTRUCT_BODY()
public:
	//Name of current node, like class, category, attribute etc.
	FString NodeName;

	FString ParentClassName;

	//current attribute for this node);
	FString Attribute;

	TWeakPtr<FGAAttributeNode> ParentNode;

	TArray<TSharedPtr<FGAAttributeNode> > ChildNodes;

	/*
		List of attributes containe within selected class (ParentClassName)
	*/
	TArray<FString> AttributeNames;
	TArray<FString> CategoryNames;
	FGAAttributeNode() {};
	~FGAAttributeNode() 
	{
		ParentClassName.Empty();
		Attribute.Empty();
		AttributeNames.Empty();
	};
};