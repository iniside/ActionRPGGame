// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystemEditor.h"

#include "GESBlueprintLibrary.h"
#include "GESEffect.h"

#include "EngineDefines.h"
#include "Editor.h"

#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2ActionMenuBuilder.h"

#include "BPNode_CreateEffect.h"

#define LOCTEXT_NAMESPACE "GameEffectSystemEditor"

//Helper which will store one of the function inputs we excpect BP callable function will have.
struct FBPNode_CreateEffectHelper
{
	static FString CauserPinName;
	static FString TargetPinName;
	static FString InstigatorPinName;
};

FString FBPNode_CreateEffectHelper::CauserPinName(TEXT("Causer"));
FString FBPNode_CreateEffectHelper::TargetPinName(TEXT("Target"));
FString FBPNode_CreateEffectHelper::InstigatorPinName(TEXT("Instigator"));

UBPNode_CreateEffect::UBPNode_CreateEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Creates a a new Effect");
}

//Adds default pins to node. These Pins (inputs ?) are always displayed.
void UBPNode_CreateEffect::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// OwningPlayer pin
	UEdGraphPin* CauserPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), AActor::StaticClass(), false, false, FBPNode_CreateEffectHelper::CauserPinName);
	SetPinToolTip(*CauserPin, LOCTEXT("CauserPinTooltip", "An actor which caused this effect to apply"));
	UEdGraphPin* TargetPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), AActor::StaticClass(), false, false, FBPNode_CreateEffectHelper::TargetPinName);
	SetPinToolTip(*TargetPin, LOCTEXT("TargetPinTooltip", "Actor which is target of ths effect"));
	UEdGraphPin* InstigatorPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), AActor::StaticClass(), false, false, FBPNode_CreateEffectHelper::InstigatorPinName);
	SetPinToolTip(*InstigatorPin, LOCTEXT("InstigatorPinTooltip", "Actor which instigated this effect. It's usually first actor in chain, like character or player controller"));

}

FLinearColor UBPNode_CreateEffect::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_CreateEffect::GetBaseNodeTitle() const
{
	return LOCTEXT("CreateEffect_BaseTitle", "Create Effect");
}

FText UBPNode_CreateEffect::GetNodeTitleFormat() const
{
	return LOCTEXT("CreateEffect", "Create {ClassName} Effect");
}

//which class can be used with this node to create objects. All childs of class can be used.
UClass* UBPNode_CreateEffect::GetClassPinBaseClass() const
{
	return UGESEffect::StaticClass();
}

//Set context menu category in which our node will be present.
FText UBPNode_CreateEffect::GetMenuCategory() const
{
	return FText::FromString("Game Effect System");
}

void UBPNode_CreateEffect::GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UBPNode_CreateEffect* TemplateNode = NewObject<UBPNode_CreateEffect>(GetTransientPackage(), GetClass());

	const FString Category = TEXT("Game Effect System");
	const FText   MenuDesc = LOCTEXT("CreateEffectMenuOption", "Create Effect...");
	const FString Tooltip = TEXT("Create a new Effect");

	TSharedPtr<FEdGraphSchemaAction_K2NewNode> NodeAction = FK2ActionMenuBuilder::AddNewNodeAction(ContextMenuBuilder, Category, MenuDesc, Tooltip);
	NodeAction->NodeTemplate = TemplateNode;
}

//gets out predefined pin
UEdGraphPin* UBPNode_CreateEffect::GetCauserPin() const
{
	UEdGraphPin* Pin = FindPin(FBPNode_CreateEffectHelper::CauserPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}
UEdGraphPin* UBPNode_CreateEffect::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(FBPNode_CreateEffectHelper::TargetPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}
UEdGraphPin* UBPNode_CreateEffect::GetInstigatorPin() const
{
	UEdGraphPin* Pin = FindPin(FBPNode_CreateEffectHelper::InstigatorPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

bool UBPNode_CreateEffect::IsSpawnVarPin(UEdGraphPin* Pin)
{
	return(Super::IsSpawnVarPin(Pin) &&
		Pin->PinName != FBPNode_CreateEffectHelper::CauserPinName
		&& Pin->PinName != FBPNode_CreateEffectHelper::TargetPinName
		&& Pin->PinName != FBPNode_CreateEffectHelper::InstigatorPinName);
}

//amd this is where magic really happens. This will expand node for our custom object, with properties
//which are set as EditAwnywhere and meta=(ExposeOnSpawn), or equivalent in blueprint.
void UBPNode_CreateEffect::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	//look for static function in BlueprintFunctionLibrary
	//In this class and of this name
	static FName Create_FunctionName = GET_FUNCTION_NAME_CHECKED(UGESBlueprintLibrary, CreateEffect);
	//with these inputs (as a Side note, these should be probabaly FName not FString)
	//static FString WorldContextObject_ParamName = FString(TEXT("WorldContextObject"));
	//static FString WidgetType_ParamName = FString(TEXT("ItemClass"));
	//static FString OwningPlayer_ParamName = FString(TEXT("OwningPlayer"));

	static const FString EffectType_Param = FString(TEXT("EffectClass"));
	static const FString CauserType_Param = FBPNode_CreateEffectHelper::CauserPinName;
	static const FString TargetType_Param = FBPNode_CreateEffectHelper::TargetPinName;
	static const FString InstigatorType_Param = FBPNode_CreateEffectHelper::InstigatorPinName;
	//get pointer to self;
	UBPNode_CreateEffect* CreateEffectNode = this;
	
	//get pointers to default pins.
	//Exec pins are those big arrows, connected with thick white lines.
	UEdGraphPin* SpawnNodeExec = CreateEffectNode->GetExecPin();
	//gets world context pin from our static function

	UEdGraphPin* SpawnCauserPin = CreateEffectNode->GetCauserPin();
	UEdGraphPin* SpawnTargetPin = CreateEffectNode->GetTargetPin();
	UEdGraphPin* SpawnInstigatorPin = CreateEffectNode->GetInstigatorPin();
	//get class pin which is used to determine which class to spawn.
	UEdGraphPin* SpawnClassPin = CreateEffectNode->GetClassPin();
	//then pin is the same as exec pin, just on the other side (the out arrow).
	UEdGraphPin* SpawnNodeThen = CreateEffectNode->GetThenPin();
	//result pin, which will output our spawned object.
	UEdGraphPin* SpawnNodeResult = CreateEffectNode->GetResultPin();

	UClass* SpawnClass = (SpawnClassPin != NULL) ? Cast<UClass>(SpawnClassPin->DefaultObject) : NULL;
	if ((0 == SpawnClassPin->LinkedTo.Num()) && (NULL == SpawnClass))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("CreateItemDAtaNodeMissingClass_Error", "Spawn node @@ must have a class specified.").ToString(), CreateEffectNode);
		// we break exec links so this is the only error we get, don't want the CreateItemData node being considered and giving 'unexpected node' type warnings
		CreateEffectNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'UWidgetBlueprintLibrary::Create' call node
	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CreateEffectNode, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(Create_FunctionName, UBPNode_CreateEffect::StaticClass());
	CallCreateNode->AllocateDefaultPins();

	//allocate nodes for created widget.
	UEdGraphPin* CallCreateExec = CallCreateNode->GetExecPin();
	UEdGraphPin* CallCreateCauserPin = CallCreateNode->FindPinChecked(CauserType_Param);
	UEdGraphPin* CallCreateTargetPin = CallCreateNode->FindPinChecked(TargetType_Param);
	UEdGraphPin* CallCreateInstigatorPin = CallCreateNode->FindPinChecked(InstigatorType_Param);
	UEdGraphPin* CallCreateEffectTypePin = CallCreateNode->FindPinChecked(EffectType_Param);
	UEdGraphPin* CallCreateResult = CallCreateNode->GetReturnValuePin();

	// Move 'exec' connection from create widget node to 'UWidgetBlueprintLibrary::Create'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallCreateExec);

	if (SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'blueprint' connection from the spawn node to 'UWidgetBlueprintLibrary::Create'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallCreateEffectTypePin);
	}
	else
	{
		// Copy blueprint literal onto 'UWidgetBlueprintLibrary::Create' call 
		CallCreateEffectTypePin->DefaultObject = SpawnClass;
	}


	// Copy the 'Owning Player' connection from the spawn node to 'UWidgetBlueprintLibrary::Create'
	CompilerContext.MovePinLinksToIntermediate(*SpawnCauserPin, *CallCreateCauserPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnTargetPin, *CallCreateTargetPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnInstigatorPin, *CallCreateInstigatorPin);

	// Move result connection from spawn node to 'UWidgetBlueprintLibrary::Create'
	CallCreateResult->PinType = SpawnNodeResult->PinType; // Copy type so it uses the right actor subclass
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallCreateResult);

	//////////////////////////////////////////////////////////////////////////
	// create 'set var' nodes

	// Get 'result' pin from 'begin spawn', this is the actual actor we want to set properties on
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, CreateEffectNode, CallCreateResult, GetClassToSpawn());

	// Move 'then' connection from create widget node to the last 'then'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen);

	// Break any links to the expanded node
	CreateEffectNode->BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE