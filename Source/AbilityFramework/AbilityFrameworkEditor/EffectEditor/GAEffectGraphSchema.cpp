// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFrameworkEditor.h"
#include "GAEffectGraphSchema.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Effects/GAGameEffect.h"
#include "Kismet2/BlueprintEditorUtils.h"

UGAEffectGraphSchema::UGAEffectGraphSchema(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UK2Node_VariableGet* UGAEffectGraphSchema::SpawnVariableGetNode(const FVector2D GraphPosition, class UEdGraph* ParentGraph, FName VariableName, UStruct* Source) const
{
	return Super::SpawnVariableGetNode(GraphPosition, ParentGraph, VariableName, Source);
}

UK2Node_VariableSet* UGAEffectGraphSchema::SpawnVariableSetNode(const FVector2D GraphPosition, class UEdGraph* ParentGraph, FName VariableName, UStruct* Source) const
{
	return Super::SpawnVariableSetNode(GraphPosition, ParentGraph, VariableName, Source);
}
