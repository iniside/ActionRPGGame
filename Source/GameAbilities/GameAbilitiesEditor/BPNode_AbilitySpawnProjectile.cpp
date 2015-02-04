// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilitiesEditor.h"

#include "GASBlueprintFunctionLibrary.h"
#include "GASAbility.h"
#include "Projectiles/GASProjectile.h"
#include "Projectiles/GASProjectileGlobals.h"

#include "EngineDefines.h"
#include "Editor.h"

#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2ActionMenuBuilder.h"
#include "BlueprintEditorUtils.h"
#include "BPNode_AbilitySpawnProjectile.h"

#define LOCTEXT_NAMESPACE "GameAbilitiesEditor"

//Helper which will store one of the function inputs we excpect BP callable function will have.
struct FUBPNode_AbilitySpawnProjectileHelper
{
	static FString EffectClassPinName;
	static FString LocationPinName;
	static FString InstigatorPinName;
};

FString FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName(TEXT("ProjectileClass"));
FString FUBPNode_AbilitySpawnProjectileHelper::LocationPinName(TEXT("Config"));
FString FUBPNode_AbilitySpawnProjectileHelper::InstigatorPinName(TEXT("AbilityInstigator"));

UBPNode_AbilitySpawnProjectile::UBPNode_AbilitySpawnProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Spawn new Projectile from ability");
}

//Adds default pins to node. These Pins (inputs ?) are always displayed.
void UBPNode_AbilitySpawnProjectile::AllocateDefaultPins()
{
	

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	// Add execution pins
	CreatePin(EGPD_Input, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Execute);
	CreatePin(EGPD_Output, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Then);

	UEdGraphPin* EffectClassPin = CreatePin(EGPD_Input, K2Schema->PC_Class, TEXT(""), AGASProjectile::StaticClass(), false, false, FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName);
	K2Schema->ConstructBasicPinTooltip(*EffectClassPin, LOCTEXT("ProjectileClassPinTooltip", "Class of Projectile to spawn."), EffectClassPin->PinToolTip);

	//UScriptStruct* ProjectileConfigStruct = FindObjectChecked<UScriptStruct>(UObject::StaticClass(), TEXT("GASProjectileConfig"));
	UEdGraphPin* LocationPin = CreatePin(EGPD_Input, K2Schema->PC_Struct, TEXT(""), FGASProjectileConfig::StaticStruct(), false, false, FUBPNode_AbilitySpawnProjectileHelper::LocationPinName);
	K2Schema->ConstructBasicPinTooltip(*LocationPin, LOCTEXT("ConfigPinTooltip", "Configuration of projectile."), LocationPin->PinToolTip);

	UEdGraphPin* InstigatorPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), UGASAbility::StaticClass(), false, false, FUBPNode_AbilitySpawnProjectileHelper::InstigatorPinName);
	K2Schema->ConstructBasicPinTooltip(*InstigatorPin, LOCTEXT("InstigatorPinTooltip", "Ability which spawned this Projectile."), InstigatorPin->PinToolTip);

	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, K2Schema->PC_Object, TEXT(""), AGASProjectile::StaticClass(), false, false, K2Schema->PN_ReturnValue);
	K2Schema->ConstructBasicPinTooltip(*ResultPin, LOCTEXT("ResultPinDescription", "The spawned Projectile"), ResultPin->PinToolTip);

	UK2Node::AllocateDefaultPins();
}

void UBPNode_AbilitySpawnProjectile::CreatePinsForClass(UClass* InClass)
{
	check(InClass != NULL);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	const UObject* const ClassDefaultObject = InClass->GetDefaultObject(false);

	for (TFieldIterator<UProperty> PropertyIt(InClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		UClass* PropertyClass = CastChecked<UClass>(Property->GetOuter());
		const bool bIsDelegate = Property->IsA(UMulticastDelegateProperty::StaticClass());
		const bool bIsExposedToSpawn = UEdGraphSchema_K2::IsPropertyExposedOnSpawn(Property);
		const bool bIsSettableExternally = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);

		if (bIsExposedToSpawn &&
			!Property->HasAnyPropertyFlags(CPF_Parm) &&
			bIsSettableExternally &&
			Property->HasAllPropertyFlags(CPF_BlueprintVisible) &&
			!bIsDelegate &&
			(NULL == FindPin(Property->GetName())))
		{
			UEdGraphPin* Pin = CreatePin(EGPD_Input, TEXT(""), TEXT(""), NULL, false, false, Property->GetName());
			const bool bPinGood = (Pin != NULL) && K2Schema->ConvertPropertyToPinType(Property, /*out*/ Pin->PinType);

			if (ClassDefaultObject && K2Schema->PinDefaultValueIsEditable(*Pin))
			{
				FString DefaultValueAsString;
				const bool bDefaultValueSet = FBlueprintEditorUtils::PropertyValueToString(Property, reinterpret_cast<const uint8*>(ClassDefaultObject), DefaultValueAsString);
				check(bDefaultValueSet);
				K2Schema->TrySetDefaultValue(*Pin, DefaultValueAsString);
			}

			// Copy tooltip from the property.
			if (Pin != nullptr)
			{
				K2Schema->ConstructBasicPinTooltip(*Pin, Property->GetToolTipText(), Pin->PinToolTip);
			}
		}
	}

	// Change class of output pin
	UEdGraphPin* ResultPin = GetResultPin();
	ResultPin->PinType.PinSubCategoryObject = InClass;
}

bool UBPNode_AbilitySpawnProjectile::IsSpawnVarPin(UEdGraphPin* Pin)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* ParentPin = Pin->ParentPin;
	return(Pin->PinName != K2Schema->PN_Execute &&
		Pin->PinName != K2Schema->PN_Then &&
		Pin->PinName != K2Schema->PN_ReturnValue &&
		Pin->PinName != FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName &&
		Pin->PinName != FUBPNode_AbilitySpawnProjectileHelper::LocationPinName &&
		Pin->PinName != FUBPNode_AbilitySpawnProjectileHelper::InstigatorPinName);
}

void UBPNode_AbilitySpawnProjectile::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (Pin && (Pin->PinName == FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName))
	{
		OnClassPinChanged();
	}
}
void UBPNode_AbilitySpawnProjectile::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin && (Pin->PinName == FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName))
	{
		OnClassPinChanged();
	}
}

UClass* UBPNode_AbilitySpawnProjectile::GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch /*=NULL*/) const
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
void UBPNode_AbilitySpawnProjectile::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	UClass* UseSpawnClass = GetClassToSpawn(&OldPins);
	if (UseSpawnClass != NULL)
	{
		CreatePinsForClass(UseSpawnClass);
	}
}

void UBPNode_AbilitySpawnProjectile::OnClassPinChanged()
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

FLinearColor UBPNode_AbilitySpawnProjectile::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_AbilitySpawnProjectile::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText NodeTitle = NSLOCTEXT("K2Node", "Projectile_BaseTitle", "Spawn Projectile from Class");
	if (TitleType != ENodeTitleType::MenuTitle)
	{
		FText SpawnString = NSLOCTEXT("K2Node", "None", "NONE");
		if (UEdGraphPin* ClassPin = GetClassPin())
		{
			if (ClassPin->LinkedTo.Num() > 0)
			{
				// Blueprint will be determined dynamically, so we don't have the name in this case
				NodeTitle = NSLOCTEXT("K2Node", "Projectile_Title_Unknown", "SpawnProjectile");
			}
			else if (ClassPin->DefaultObject == nullptr)
			{
				NodeTitle = NSLOCTEXT("K2Node", "Projectile_Title_NONE", "Projectile NONE");
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
					CachedNodeTitle = FText::Format(NSLOCTEXT("K2Node", "Projectile", "SpawnProjectile {ClassName}"), Args);
				}
				NodeTitle = CachedNodeTitle;
			}
		}
		else
		{
			NodeTitle = NSLOCTEXT("K2Node", "Projectile_Title_NONE", "Projectile NONE");
		}
	}
	return NodeTitle;
}


bool UBPNode_AbilitySpawnProjectile::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	//UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	//return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph);
	return true;
}


FText UBPNode_AbilitySpawnProjectile::GetTooltipText() const
{
	return FText::FromString("Create new Projectile");
}
//Set context menu category in which our node will be present.
FText UBPNode_AbilitySpawnProjectile::GetMenuCategory() const
{
	return FText::FromString("Game Ability System");
}

void UBPNode_AbilitySpawnProjectile::GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UBPNode_AbilitySpawnProjectile* TemplateNode = NewObject<UBPNode_AbilitySpawnProjectile>(GetTransientPackage(), GetClass());

	const FString Category = TEXT("Game Ability System");
	const FText   MenuDesc = LOCTEXT("CreateEffectFieldMenuOption", "Spawn Projectile");
	const FString Tooltip = TEXT("Spawn a new Projectile");

	TSharedPtr<FEdGraphSchemaAction_K2NewNode> NodeAction = FK2ActionMenuBuilder::AddNewNodeAction(ContextMenuBuilder, Category, MenuDesc, Tooltip);
	NodeAction->NodeTemplate = TemplateNode;
}
UEdGraphPin* UBPNode_AbilitySpawnProjectile::GetThenPin() const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}
//gets out predefined pin
UEdGraphPin* UBPNode_AbilitySpawnProjectile::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = NULL;
	for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
	{
		UEdGraphPin* TestPin = *PinIt;
		if (TestPin && TestPin->PinName == FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}
UEdGraphPin* UBPNode_AbilitySpawnProjectile::GetLocationPin() const
{
	UEdGraphPin* Pin = FindPin(FUBPNode_AbilitySpawnProjectileHelper::LocationPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UBPNode_AbilitySpawnProjectile::GetInstigatorPin() const
{
	UEdGraphPin* Pin = FindPin(FUBPNode_AbilitySpawnProjectileHelper::InstigatorPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UBPNode_AbilitySpawnProjectile::GetResultPin() const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

void UBPNode_AbilitySpawnProjectile::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	UClass* ClassToSpawn = GetClassToSpawn();
	const FString ClassToSpawnStr = ClassToSpawn ? ClassToSpawn->GetName() : TEXT("InvalidClass");
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("SpawnProjectileFromClass")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("ActorClass"), ClassToSpawnStr));
}

//amd this is where magic really happens. This will expand node for our custom object, with properties
//which are set as EditAwnywhere and meta=(ExposeOnSpawn), or equivalent in blueprint.
void UBPNode_AbilitySpawnProjectile::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	UK2Node::ExpandNode(CompilerContext, SourceGraph);

	//look for static function in BlueprintFunctionLibrary
	//In this class and of this name
	static FName BeginSpawn_FunctionName = GET_FUNCTION_NAME_CHECKED(UGASBlueprintFunctionLibrary, BeginSpawnProjectile);
	static FName FinishSpawn_FunctionName = GET_FUNCTION_NAME_CHECKED(UGASBlueprintFunctionLibrary, FinishSpawnProjectile);

	static FString EffectFieldParamName = FString(TEXT("Projectile"));
	//with these inputs (as a Side note, these should be probabaly FName not FString)

	static const FString EffectClassType_Param = FUBPNode_AbilitySpawnProjectileHelper::EffectClassPinName;
	static const FString LocationType_Param = FUBPNode_AbilitySpawnProjectileHelper::LocationPinName;
	static const FString InstigatorType_Param = FUBPNode_AbilitySpawnProjectileHelper::InstigatorPinName;
	//get pointer to self;
	UBPNode_AbilitySpawnProjectile* CreateEffectNode = this;
	
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
	// create 'begin spawn' call node
	UK2Node_CallFunction* CallBeginCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CreateEffectNode, SourceGraph);
	CallBeginCreateNode->FunctionReference.SetExternalMember(BeginSpawn_FunctionName, UBlueprintFunctionLibrary::StaticClass());
	CallBeginCreateNode->AllocateDefaultPins();

	//allocate nodes for 'begin spawn' call node
	UEdGraphPin* CallBeginExec = CallBeginCreateNode->GetExecPin();
	UEdGraphPin* CallBeginClassPin = CallBeginCreateNode->FindPinChecked(EffectClassType_Param);
	UEdGraphPin* CallBeginLocationPin = CallBeginCreateNode->FindPinChecked(LocationType_Param);
	UEdGraphPin* CallBeginInstigatorPin = CallBeginCreateNode->FindPinChecked(InstigatorType_Param);
	UEdGraphPin* CallBeginResult = CallBeginCreateNode->GetReturnValuePin();

	// Move 'exec' connection from spawn node to 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallBeginExec);

	if (SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'blueprint' connection from the spawn node to 'begin spawn'
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