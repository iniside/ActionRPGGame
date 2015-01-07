// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystemEditor.h"

#include "GSBlueprintFunctionLibrary.h"

#include "EffectField/GSEffectField.h"

#include "EngineDefines.h"
#include "Editor.h"

#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2ActionMenuBuilder.h"
#include "BlueprintEditorUtils.h"
#include "BPNode_SpawnEffectField.h"

#define LOCTEXT_NAMESPACE "GameAbilitiesEditor"

//Helper which will store one of the function inputs we excpect BP callable function will have.
struct FBPNode_SpawnEffectFieldHelper
{
	static FString EffectClassPinName;
	static FString LocationPinName;
	static FString InstigatorPinName;
};

FString FBPNode_SpawnEffectFieldHelper::EffectClassPinName(TEXT("EffectFieldClass"));
FString FBPNode_SpawnEffectFieldHelper::LocationPinName(TEXT("Location"));
FString FBPNode_SpawnEffectFieldHelper::InstigatorPinName(TEXT("FieldInstigator"));

UBPNode_SpawnEffectField::UBPNode_SpawnEffectField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Create new effect field from ability");
}

//Adds default pins to node. These Pins (inputs ?) are always displayed.
void UBPNode_SpawnEffectField::AllocateDefaultPins()
{
	UK2Node::AllocateDefaultPins();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	// Add execution pins
	CreatePin(EGPD_Input, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Execute);
	CreatePin(EGPD_Output, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Then);

	UEdGraphPin* EffectClassPin = CreatePin(EGPD_Input, K2Schema->PC_Class, TEXT(""), AGSEffectField::StaticClass(), false, false, FBPNode_SpawnEffectFieldHelper::EffectClassPinName);
	K2Schema->ConstructBasicPinTooltip(*EffectClassPin, LOCTEXT("EffectClassPinTooltip", "Class of Effect field to spawn"), EffectClassPin->PinToolTip);

	UScriptStruct* VectorStruct = FindObjectChecked<UScriptStruct>(UObject::StaticClass(), TEXT("Vector"));
	UEdGraphPin* LocationPin = CreatePin(EGPD_Input, K2Schema->PC_Struct, TEXT(""), VectorStruct, false, false, FBPNode_SpawnEffectFieldHelper::LocationPinName);
	K2Schema->ConstructBasicPinTooltip(*LocationPin, LOCTEXT("LocationPinTooltip", "Location at which Effect Field will be spawned"), LocationPin->PinToolTip);

	UEdGraphPin* InstigatorPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), AActor::StaticClass(), false, false, FBPNode_SpawnEffectFieldHelper::InstigatorPinName);
	K2Schema->ConstructBasicPinTooltip(*InstigatorPin, LOCTEXT("InstigatorPinTooltip", "Actor which spawned this field. Must implement IIGSEffectField Interface."), InstigatorPin->PinToolTip);

	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, K2Schema->PC_Object, TEXT(""), AGSEffectField::StaticClass(), false, false, K2Schema->PN_ReturnValue);
	K2Schema->ConstructBasicPinTooltip(*ResultPin, LOCTEXT("ResultPinDescription", "The  spawned Effect Field"), ResultPin->PinToolTip);


}

bool UBPNode_SpawnEffectField::IsSpawnVarPin(UEdGraphPin* Pin)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* ParentPin = Pin->ParentPin;
	return(Pin->PinName != K2Schema->PN_Execute &&
		Pin->PinName != K2Schema->PN_Then &&
		Pin->PinName != K2Schema->PN_ReturnValue &&
		Pin->PinName != FBPNode_SpawnEffectFieldHelper::EffectClassPinName &&
		Pin->PinName != FBPNode_SpawnEffectFieldHelper::LocationPinName &&
		Pin->PinName != FBPNode_SpawnEffectFieldHelper::InstigatorPinName);
}

void UBPNode_SpawnEffectField::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (Pin && (Pin->PinName == FBPNode_SpawnEffectFieldHelper::EffectClassPinName))
	{
		OnClassPinChanged();
	}
}
void UBPNode_SpawnEffectField::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin && (Pin->PinName == FBPNode_SpawnEffectFieldHelper::EffectClassPinName))
	{
		OnClassPinChanged();
	}
}

UClass* UBPNode_SpawnEffectField::GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch /*=NULL*/) const
{
	UClass* UseSpawnClass = NULL;
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* ClassPin = GetClassPin(PinsToSearch);
	if (ClassPin && ClassPin->DefaultObject != NULL && ClassPin->LinkedTo.Num() == 0)
	{
		UseSpawnClass = CastChecked<UClass>(ClassPin->DefaultObject);
	}
	else if (ClassPin && (1 == ClassPin->LinkedTo.Num()))
	{
		auto SourcePin = ClassPin->LinkedTo[0];
		UseSpawnClass = SourcePin ? Cast<UClass>(SourcePin->PinType.PinSubCategoryObject.Get()) : NULL;
	}

	return UseSpawnClass;
}

void UBPNode_SpawnEffectField::OnClassPinChanged()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Because the archetype has changed, we break the output link as the output pin type will change
	UEdGraphPin* ResultPin = GetResultPin();
	ResultPin->BreakAllPinLinks();

	// Remove all pins related to archetype variables
	TArray<UEdGraphPin*> OldPins = Pins;
	for (int32 i = 0; i < OldPins.Num(); i++)
	{
		UEdGraphPin* OldPin = OldPins[i];
		if (IsSpawnVarPin(OldPin))
		{
			OldPin->BreakAllPinLinks();
			Pins.Remove(OldPin);
		}
	}

	CachedNodeTitle.MarkDirty();

	UClass* UseSpawnClass = GetClassToSpawn();
	if (UseSpawnClass != NULL)
	{
		CreatePinsForClass(UseSpawnClass);
	}
	K2Schema->ConstructBasicPinTooltip(*ResultPin, LOCTEXT("ResultPinDescription", "The spawned Actor"), ResultPin->PinToolTip);

	// Refresh the UI for the graph so the pin changes show up
	UEdGraph* Graph = GetGraph();
	Graph->NotifyGraphChanged();

	// Mark dirty
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

FLinearColor UBPNode_SpawnEffectField::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_SpawnEffectField::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText NodeTitle = NSLOCTEXT("K2Node", "EffectField_BaseTitle", "Spawn Effect Field from Class");
	if (TitleType != ENodeTitleType::MenuTitle)
	{
		FText SpawnString = NSLOCTEXT("K2Node", "None", "NONE");
		if (UEdGraphPin* ClassPin = GetClassPin())
		{
			if (ClassPin->LinkedTo.Num() > 0)
			{
				// Blueprint will be determined dynamically, so we don't have the name in this case
				NodeTitle = NSLOCTEXT("K2Node", "EffectField_Title_Unknown", "SpawnEffectField");
			}
			else if (ClassPin->DefaultObject == nullptr)
			{
				NodeTitle = NSLOCTEXT("K2Node", "EffectField_Title_NONE", "EffectField NONE");
			}
			else
			{
				if (CachedNodeTitle.IsOutOfDate())
				{
					FText ClassName;
					if (UClass* PickedClass = Cast<UClass>(ClassPin->DefaultObject))
					{
						ClassName = PickedClass->GetDisplayNameText();
					}

					FFormatNamedArguments Args;
					Args.Add(TEXT("ClassName"), ClassName);

					// FText::Format() is slow, so we cache this to save on performance
					CachedNodeTitle = FText::Format(NSLOCTEXT("K2Node", "EffectField", "SpawnEffectField {ClassName}"), Args);
				}
				NodeTitle = CachedNodeTitle;
			}
		}
		else
		{
			NodeTitle = NSLOCTEXT("K2Node", "EffectField_Title_NONE", "EffectField NONE");
		}
	}
	return NodeTitle;
}
FText UBPNode_SpawnEffectField::GetTooltipText() const
{
	return FText::FromString("Create new Effect Field");
}
//Set context menu category in which our node will be present.
FText UBPNode_SpawnEffectField::GetMenuCategory() const
{
	return FText::FromString("Game System|Effect Field");
}

void UBPNode_SpawnEffectField::GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UBPNode_SpawnEffectField* TemplateNode = NewObject<UBPNode_SpawnEffectField>(GetTransientPackage(), GetClass());

	const FString Category = TEXT("Game System");
	const FText   MenuDesc = LOCTEXT("CreateEffectFieldMenuOption", "Create Effect Field");
	const FString Tooltip = TEXT("Create a new Effect Field");

	TSharedPtr<FEdGraphSchemaAction_K2NewNode> NodeAction = FK2ActionMenuBuilder::AddNewNodeAction(ContextMenuBuilder, Category, MenuDesc, Tooltip);
	NodeAction->NodeTemplate = TemplateNode;
}
UEdGraphPin* UBPNode_SpawnEffectField::GetThenPin() const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}
//gets out predefined pin
UEdGraphPin* UBPNode_SpawnEffectField::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = NULL;
	for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
	{
		UEdGraphPin* TestPin = *PinIt;
		if (TestPin && TestPin->PinName == FBPNode_SpawnEffectFieldHelper::EffectClassPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}
UEdGraphPin* UBPNode_SpawnEffectField::GetLocationPin() const
{
	UEdGraphPin* Pin = FindPin(FBPNode_SpawnEffectFieldHelper::LocationPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UBPNode_SpawnEffectField::GetInstigatorPin() const
{
	UEdGraphPin* Pin = FindPin(FBPNode_SpawnEffectFieldHelper::InstigatorPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UBPNode_SpawnEffectField::GetResultPin() const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

void UBPNode_SpawnEffectField::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	UClass* ClassToSpawn = GetClassToSpawn();
	const FString ClassToSpawnStr = ClassToSpawn ? ClassToSpawn->GetName() : TEXT("InvalidClass");
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("SpawnEffectFieldFromClass")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("ActorClass"), ClassToSpawnStr));
}

//amd this is where magic really happens. This will expand node for our custom object, with properties
//which are set as EditAwnywhere and meta=(ExposeOnSpawn), or equivalent in blueprint.
void UBPNode_SpawnEffectField::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	UK2Node::ExpandNode(CompilerContext, SourceGraph);

	//look for static function in BlueprintFunctionLibrary
	//In this class and of this name
	static FName BeginSpawn_FunctionName = GET_FUNCTION_NAME_CHECKED(UGSBlueprintFunctionLibrary, BeginCreateEffectField);


	static FName FinishSpawn_FunctionName = GET_FUNCTION_NAME_CHECKED(UGSBlueprintFunctionLibrary, FinishCreateEffectField);
	static FString EffectFieldParamName = FString(TEXT("EffectField"));
	//static FName Create_FunctionName = GET_FUNCTION_NAME_CHECKED(UGESEffectManager, SpawnEffect);
	//with these inputs (as a Side note, these should be probabaly FName not FString)

	static const FString EffectClassType_Param = FBPNode_SpawnEffectFieldHelper::EffectClassPinName;
	static const FString LocationType_Param = FBPNode_SpawnEffectFieldHelper::LocationPinName;
	static const FString InstigatorType_Param = FBPNode_SpawnEffectFieldHelper::InstigatorPinName;
	//get pointer to self;
	UBPNode_SpawnEffectField* CreateEffectNode = this;
	
	//get pointers to default pins.
	//Exec pins are those big arrows, connected with thick white lines.
	UEdGraphPin* SpawnNodeExec = CreateEffectNode->GetExecPin();
	//gets world context pin from our static function

	UEdGraphPin* SpawnClassPin = CreateEffectNode->GetClassPin();
	UEdGraphPin* SpawnLocationPin = CreateEffectNode->GetLocationPin();
	UEdGraphPin* SpawnInstigatorPin = CreateEffectNode->GetInstigatorPin();
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
	UK2Node_CallFunction* CallBeginCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CreateEffectNode, SourceGraph);
	CallBeginCreateNode->FunctionReference.SetExternalMember(BeginSpawn_FunctionName, UBlueprintFunctionLibrary::StaticClass());
	CallBeginCreateNode->AllocateDefaultPins();

	//allocate nodes for created widget.
	UEdGraphPin* CallBeginExec = CallBeginCreateNode->GetExecPin();
	UEdGraphPin* CallBeginClassPin = CallBeginCreateNode->FindPinChecked(EffectClassType_Param);
	UEdGraphPin* CallBeginLocationPin = CallBeginCreateNode->FindPinChecked(LocationType_Param);
	UEdGraphPin* CallBeginInstigatorPin = CallBeginCreateNode->FindPinChecked(InstigatorType_Param);
	UEdGraphPin* CallBeginResult = CallBeginCreateNode->GetReturnValuePin();

	// Move 'exec' connection from create widget node to 'UWidgetBlueprintLibrary::Create'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallBeginExec);

	if (SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'blueprint' connection from the spawn node to 'UWidgetBlueprintLibrary::Create'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallBeginClassPin);
	}
	else
	{
		// Copy blueprint literal onto 'UWidgetBlueprintLibrary::Create' call 
		CallBeginClassPin->DefaultObject = SpawnClass;
	}
	CompilerContext.MovePinLinksToIntermediate(*SpawnLocationPin, *CallBeginLocationPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnInstigatorPin, *CallBeginInstigatorPin);

	UK2Node_CallFunction* CallFinishSpawnNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CreateEffectNode, SourceGraph);
	CallFinishSpawnNode->FunctionReference.SetExternalMember(FinishSpawn_FunctionName, UBlueprintFunctionLibrary::StaticClass());
	CallFinishSpawnNode->AllocateDefaultPins();

	UEdGraphPin* CallFinishExec = CallFinishSpawnNode->GetExecPin();
	UEdGraphPin* CallFinishThen = CallFinishSpawnNode->GetThenPin();
	UEdGraphPin* CallFinishActor = CallFinishSpawnNode->FindPinChecked(EffectFieldParamName);
	UEdGraphPin* CallFinishResult = CallFinishSpawnNode->GetReturnValuePin();

	// Move 'then' connection from spawn node to 'finish spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *CallFinishThen);

	// Connect output actor from 'begin' to 'finish'
	CallBeginResult->MakeLinkTo(CallFinishActor);

	// Move result connection from spawn node to 'finish spawn'
	CallFinishResult->PinType = SpawnNodeResult->PinType; // Copy type so it uses the right actor subclass
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallFinishResult);


	//////////////////////////////////////////////////////////////////////////
	// create 'set var' nodes

	// Get 'result' pin from 'begin spawn', this is the actual actor we want to set properties on
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallBeginCreateNode, CreateEffectNode, CallBeginResult, GetClassToSpawn());

	// Move 'then' connection from create widget node to the last 'then'
	LastThen->MakeLinkTo(CallFinishExec);

	// Break any links to the expanded node
	CreateEffectNode->BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE