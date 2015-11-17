
#include "GameAbilitiesEditor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "GameplayTask.h"
#include "Tasks/GASAbilityTask.h"
#include "Entity/GASAbilityMod.h"
#include "GASBlueprintFunctionLibrary.h"
#include "GASAbilityBase.h"
#include "KismetCompiler.h"
#include "BlueprintEditorUtils.h"
#include "GAEK2Node_LatentAbilityTaskCall.h"
#include "K2Node_EnumLiteral.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "K2Node"

UGAEK2Node_LatentAbilityTaskCall::UGAEK2Node_LatentAbilityTaskCall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == true)
	{
		UK2Node_LatentGameplayTaskCall::RegisterSpecializedTaskNodeClass(GetClass());
	}
}

bool UGAEK2Node_LatentAbilityTaskCall::IsHandling(TSubclassOf<UGameplayTask> TaskClass) const
{
	bool isChilldOf = TaskClass && TaskClass->IsChildOf(UGASAbilityTask::StaticClass());
	return isChilldOf;
}

bool UGAEK2Node_LatentAbilityTaskCall::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = false;

	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);

	if (bAllowLatentFuncs)
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
		if (MyBlueprint && MyBlueprint->GeneratedClass)
		{
			if (MyBlueprint->GeneratedClass->IsChildOf(UGASAbilityBase::StaticClass()))
			{
				bIsCompatible = true;
			}
		}
	}
	return bIsCompatible;
}


void UGAEK2Node_LatentAbilityTaskCall::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//UK2Node_BaseAsyncTask::GetMenuActions(ActionRegistrar);
	struct GetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			UGAEK2Node_LatentAbilityTaskCall* AsyncTaskNode = CastChecked<UGAEK2Node_LatentAbilityTaskCall>(NewNode);
			if (FunctionPtr.IsValid())
			{
				UFunction* Func = FunctionPtr.Get();
				UObjectProperty* ReturnProp = CastChecked<UObjectProperty>(Func->GetReturnProperty());
						
				AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
				AsyncTaskNode->ProxyFactoryClass        = Func->GetOuterUClass();
				AsyncTaskNode->ProxyClass               = ReturnProp->PropertyClass;
			}
		}
	};

	UClass* NodeClass = GetClass();
	//FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateUObject(this, &UGAEK2Node_LatentAbilityTaskCall::CreateNodeSpawner);
	ActionRegistrar.RegisterClassFactoryActions<UGASAbilityTask>(FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintFunctionNodeSpawner::Create(FactoryFunc);
		check(NodeSpawner != nullptr);
		NodeSpawner->NodeClass = NodeClass;

		TWeakObjectPtr<UFunction> FunctionPtr = FactoryFunc;
		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(GetMenuActions_Utils::SetNodeFunc, FunctionPtr);
		return NodeSpawner;
	}));
}

//UEdGraphPin* UGAEK2Node_LatentAbilityTaskCall::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
//{
//	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
//
//	UEdGraphPin* Pin = NULL;
//	for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
//	{
//		UEdGraphPin* TestPin = *PinIt;
//		if (TestPin && TestPin->PinName == "Class" || TestPin->PinName == "InClass")
//		{
//			Pin = TestPin;
//			break;
//		}
//	}
//	check(Pin == NULL || Pin->Direction == EGPD_Input);
//	return Pin;
//}
//
//UClass* UGAEK2Node_LatentAbilityTaskCall::GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch) const
//{
//	UClass* UseSpawnClass = NULL;
//	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
//
//	UEdGraphPin* ClassPin = GetClassPin(PinsToSearch);
//	if (ClassPin && ClassPin->DefaultObject != NULL && ClassPin->LinkedTo.Num() == 0)
//	{
//		UseSpawnClass = CastChecked<UClass>(ClassPin->DefaultObject);
//	}
//
//	return UseSpawnClass;
//}

//struct FK2Node_LatentAbilityCallHelper
//{
//	static FString WorldContextPinName;
//	static FString ClassPinName;
//	static FString BeginSpawnFuncName;
//	static FString FinishSpawnFuncName;
//	static FString BeginSpawnArrayFuncName;
//	static FString FinishSpawnArrayFuncName;
//	static FString SpawnedActorPinName;
//};
//
//FString FK2Node_LatentAbilityCallHelper::WorldContextPinName(TEXT("WorldContextObject"));
//FString FK2Node_LatentAbilityCallHelper::ClassPinName(TEXT("Class"));
//FString FK2Node_LatentAbilityCallHelper::BeginSpawnFuncName(TEXT("BeginSpawningActor"));
//FString FK2Node_LatentAbilityCallHelper::FinishSpawnFuncName(TEXT("FinishSpawningActor"));
//FString FK2Node_LatentAbilityCallHelper::BeginSpawnArrayFuncName(TEXT("BeginSpawningActorArray"));
//FString FK2Node_LatentAbilityCallHelper::FinishSpawnArrayFuncName(TEXT("FinishSpawningActorArray"));
//FString FK2Node_LatentAbilityCallHelper::SpawnedActorPinName(TEXT("SpawnedActor"));
//
///**
//*	This is essentially a mix of K2Node_BaseAsyncTask::ExpandNode and K2Node_SpawnActorFromClass::ExpandNode.
//*	Several things are going on here:
//*		-Factory call to create proxy object (K2Node_BaseAsyncTask)
//*		-Task return delegates are created and hooked up (K2Node_BaseAsyncTask)
//*		-A BeginSpawn function is called on proxyu object (similiar to K2Node_SpawnActorFromClass)
//*		-BeginSpawn can choose to spawn or not spawn an actor (and return it)
//*			-If spawned:
//*				-SetVars are run on the newly spawned object (set expose on spawn variables - K2Node_SpawnActorFromClass)
//*				-FinishSpawn is called on the proxy object
//*
//*
//*	Also, a K2Node_SpawnActorFromClass could not be used directly here, since we want the proxy object to implement its own
//*	BeginSpawn/FinishSpawn function (custom game logic will often be performed in the native implementation). K2Node_SpawnActorFromClass also
//*	requires a SpawnTransform be wired into it, and in most ability task cases, the spawn transform is implied or not necessary.
//*
//*	iniside edit:
//*	Most of this code is ripped of from parent class, because I also need UObject spawning, along with actor.
//*/
//void UGAEK2Node_LatentAbilityTaskCall::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
//{
//	UEdGraphPin* ClassPin = GetClassPin();
//	const TArray<UEdGraphPin*>* PinsToSearch = &Pins;
//
//	UEdGraphPin* ObjPin = NULL;
//	for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
//	{
//		UEdGraphPin* TestPin = *PinIt;
//		if (TestPin && TestPin->PinName == "InClass")
//		{
//			ObjPin = TestPin;
//			break;
//		}
//	}
//	
//	if (ClassPin == nullptr && ObjPin == nullptr)
//	{
//		// Nothing special about this task, just call super
//		Super::ExpandNode(CompilerContext, SourceGraph);
//		return;
//	}
//
//	UK2Node::ExpandNode(CompilerContext, SourceGraph);
//
//	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
//	check(SourceGraph && Schema);
//	bool bIsErrorFree = true;
//
//
//	// ------------------------------------------------------------------------------------------
//	// CREATE A CALL TO FACTORY THE PROXY OBJECT
//	// ------------------------------------------------------------------------------------------
//	UK2Node_CallFunction* const CallCreateProxyObjectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//	CallCreateProxyObjectNode->FunctionReference.SetExternalMember(ProxyFactoryFunctionName, ProxyFactoryClass);
//	CallCreateProxyObjectNode->AllocateDefaultPins();
//	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(Schema->PN_Execute), *CallCreateProxyObjectNode->FindPinChecked(Schema->PN_Execute)).CanSafeConnect();
//	for (auto CurrentPin : Pins)
//	{
//		if (FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Input, Schema))
//		{
//			UEdGraphPin* DestPin = CallCreateProxyObjectNode->FindPin(CurrentPin->PinName); // match function inputs, to pass data to function from CallFunction node
//
//																							// NEW: if no DestPin, assume it is a Class Spawn PRoperty - not an error
//			if (DestPin)
//			{
//				bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*CurrentPin, *DestPin).CanSafeConnect();
//			}
//		}
//	}
//
//	// Expose Async Task Proxy object
//	UEdGraphPin* const ProxyObjectPin = CallCreateProxyObjectNode->GetReturnValuePin();
//	check(ProxyObjectPin);
//	auto OutputAsyncTaskProxy = FindPinChecked(FBaseAsyncTaskHelper::GetAsyncTaskProxyName());
//	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OutputAsyncTaskProxy, *ProxyObjectPin).CanSafeConnect();
//
//	// ------------------------------------------------------------------------------------------
//	// GATHER OUTPUT PARAMETERS AND PAIR THEM WITH LOCAL VARIABLES
//	// ------------------------------------------------------------------------------------------
//	TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable> VariableOutputs;
//	for (auto CurrentPin : Pins)
//	{
//		if ((OutputAsyncTaskProxy != CurrentPin) && FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Output, Schema))
//		{
//			const FEdGraphPinType& PinType = CurrentPin->PinType;
//			UK2Node_TemporaryVariable* TempVarOutput = CompilerContext.SpawnInternalVariable(
//				this, PinType.PinCategory, PinType.PinSubCategory, PinType.PinSubCategoryObject.Get(), PinType.bIsArray);
//			bIsErrorFree &= TempVarOutput->GetVariablePin() && CompilerContext.MovePinLinksToIntermediate(*CurrentPin, *TempVarOutput->GetVariablePin()).CanSafeConnect();
//			VariableOutputs.Add(FBaseAsyncTaskHelper::FOutputPinAndLocalVariable(CurrentPin, TempVarOutput));
//		}
//	}
//
//	// ------------------------------------------------------------------------------------------
//	// FOR EACH DELEGATE DEFINE EVENT, CONNECT IT TO DELEGATE AND IMPLEMENT A CHAIN OF ASSIGMENTS
//	// ------------------------------------------------------------------------------------------
//	UEdGraphPin* LastThenPin = CallCreateProxyObjectNode->FindPinChecked(Schema->PN_Then);
//	for (TFieldIterator<UMulticastDelegateProperty> PropertyIt(ProxyClass, EFieldIteratorFlags::ExcludeSuper); PropertyIt && bIsErrorFree; ++PropertyIt)
//	{
//		bIsErrorFree &= FBaseAsyncTaskHelper::HandleDelegateImplementation(*PropertyIt, VariableOutputs, ProxyObjectPin, LastThenPin, this, SourceGraph, CompilerContext);
//	}
//
//	if (CallCreateProxyObjectNode->FindPinChecked(Schema->PN_Then) == LastThenPin)
//	{
//		CompilerContext.MessageLog.Error(*LOCTEXT("MissingDelegateProperties", "BaseAsyncTask: Proxy has no delegates defined. @@").ToString(), this);
//		return;
//	}
//
//
//	/*UClass* IsActor = GetClassToSpawn();
//	if (IsActor && IsActor->GetDefaultObject()->IsA(AActor::StaticClass()))
//	{*/
//		bool bValidatedActorSpawn = ValidateActorSpawning(CompilerContext, false);
//		bool bValidatedActorArraySpawn = ValidateActorArraySpawning(CompilerContext, false);
//		if (!bValidatedActorSpawn && !bValidatedActorArraySpawn)
//		{
//			ValidateActorSpawning(CompilerContext, true);
//			ValidateActorArraySpawning(CompilerContext, true);
//		}
//
//		// ------------------------------------------------------------------------------------------
//		// NEW: CREATE A CALL TO THE PRESPAWN FUNCTION, IF IT RETURNS TRUE, THEN WE WILL SPAWN THE NEW ACTOR
//		// ------------------------------------------------------------------------------------------
//
//		FName ProxyPrespawnFunctionName = bValidatedActorArraySpawn ? *FK2Node_LatentAbilityCallHelper::BeginSpawnArrayFuncName : *FK2Node_LatentAbilityCallHelper::BeginSpawnFuncName;
//		UFunction* PreSpawnFunction = ProxyFactoryClass->FindFunctionByName(ProxyPrespawnFunctionName);
//
//		FName ProxyPostpawnFunctionName = bValidatedActorArraySpawn ? *FK2Node_LatentAbilityCallHelper::FinishSpawnArrayFuncName : *FK2Node_LatentAbilityCallHelper::FinishSpawnFuncName;
//		UFunction* PostSpawnFunction = ProxyFactoryClass->FindFunctionByName(ProxyPostpawnFunctionName);
//
//		if (PreSpawnFunction == nullptr)
//		{
//			if (bValidatedActorArraySpawn)
//			{
//				CompilerContext.MessageLog.Error(*LOCTEXT("MissingBeginSpawningActorArrayFunction", "AbilityTask: Proxy is missing BeginSpawningActorArray native function. @@").ToString(), this);
//			}
//			else
//			{
//				CompilerContext.MessageLog.Error(*LOCTEXT("MissingBeginSpawningActorFunction", "AbilityTask: Proxy is missing BeginSpawningActor native function. @@").ToString(), this);
//			}
//			return;
//		}
//
//		if (PostSpawnFunction == nullptr)
//		{
//			if (bValidatedActorArraySpawn)
//			{
//				CompilerContext.MessageLog.Error(*LOCTEXT("MissingFinishSpawningActorArrayFunction", "AbilityTask: Proxy is missing FinishSpawningActorArray native function. @@").ToString(), this);
//			}
//			else
//			{
//				CompilerContext.MessageLog.Error(*LOCTEXT("MissingFinishSpawningActorFunction", "AbilityTask: Proxy is missing FinishSpawningActor native function. @@").ToString(), this);
//			}
//			return;
//		}
//
//
//		UK2Node_CallFunction* const CallPrespawnProxyObjectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//		CallPrespawnProxyObjectNode->FunctionReference.SetExternalMember(ProxyPrespawnFunctionName, ProxyClass);
//		CallPrespawnProxyObjectNode->AllocateDefaultPins();
//
//		// Hook up the self connection
//		UEdGraphPin* PrespawnCallSelfPin = Schema->FindSelfPin(*CallPrespawnProxyObjectNode, EGPD_Input);
//		check(PrespawnCallSelfPin);
//
//		bIsErrorFree &= Schema->TryCreateConnection(ProxyObjectPin, PrespawnCallSelfPin);
//
//		// Hook up input parameters to PreSpawn
//		for (auto CurrentPin : Pins)
//		{
//			if (FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Input, Schema))
//			{
//				UEdGraphPin* DestPin = CallPrespawnProxyObjectNode->FindPin(CurrentPin->PinName);
//				if (DestPin)
//				{
//					bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*CurrentPin, *DestPin).CanSafeConnect();
//				}
//			}
//		}
//
//		// Hook the activate node up in the exec chain
//		UEdGraphPin* PrespawnExecPin = CallPrespawnProxyObjectNode->FindPinChecked(Schema->PN_Execute);
//		UEdGraphPin* PrespawnThenPin = CallPrespawnProxyObjectNode->FindPinChecked(Schema->PN_Then);
//		UEdGraphPin* PrespawnReturnPin = CallPrespawnProxyObjectNode->FindPinChecked(Schema->PN_ReturnValue);
//		UEdGraphPin* SpawnedActorReturnPin = CallPrespawnProxyObjectNode->FindPinChecked(FK2Node_LatentAbilityCallHelper::SpawnedActorPinName);
//
//		bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, PrespawnExecPin);
//
//		LastThenPin = PrespawnThenPin;
//
//		// -------------------------------------------
//		// Branch based on return value of Prespawn
//		// -------------------------------------------
//
//		UK2Node_IfThenElse* BranchNode = SourceGraph->CreateBlankNode<UK2Node_IfThenElse>();
//		BranchNode->AllocateDefaultPins();
//		CompilerContext.MessageLog.NotifyIntermediateObjectCreation(BranchNode, this);
//
//		// Link return value of prespawn with the branch condtional
//		bIsErrorFree &= Schema->TryCreateConnection(PrespawnReturnPin, BranchNode->GetConditionPin());
//
//		// Link our Prespawn call to the branch node
//		bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, BranchNode->GetExecPin());
//
//		UEdGraphPin* BranchElsePin = BranchNode->GetElsePin();
//
//		LastThenPin = BranchNode->GetThenPin();
//
//		UClass* ClassToSpawn = GetClassToSpawn();
//		if (bValidatedActorArraySpawn && ClassToSpawn)
//		{
//			//Branch for main loop control
//			UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
//			Branch->AllocateDefaultPins();
//
//			//Create int Iterator
//			UK2Node_TemporaryVariable* IteratorVar = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(this, SourceGraph);
//			IteratorVar->VariableType.PinCategory = Schema->PC_Int;
//			IteratorVar->AllocateDefaultPins();
//
//			//Iterator assignment (initialization to zero)
//			UK2Node_AssignmentStatement* IteratorInitialize = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
//			IteratorInitialize->AllocateDefaultPins();
//			IteratorInitialize->GetValuePin()->DefaultValue = TEXT("0");
//
//			//Iterator assignment (incrementing)
//			UK2Node_AssignmentStatement* IteratorAssign = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
//			IteratorAssign->AllocateDefaultPins();
//
//			//Increment iterator command
//			UK2Node_CallFunction* Increment = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//			Increment->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, Add_IntInt)));
//			Increment->AllocateDefaultPins();
//			Increment->FindPinChecked(TEXT("B"))->DefaultValue = TEXT("1");
//
//			//Array length
//			UK2Node_CallArrayFunction* ArrayLength = CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(this, SourceGraph);
//			ArrayLength->SetFromFunction(UKismetArrayLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetArrayLibrary, Array_Length)));
//			ArrayLength->AllocateDefaultPins();
//
//			//Array element retrieval
//			UK2Node_CallArrayFunction* GetElement = CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(this, SourceGraph);
//			GetElement->SetFromFunction(UKismetArrayLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetArrayLibrary, Array_Get)));
//			GetElement->AllocateDefaultPins();
//
//			//Check node for iterator versus array length
//			UK2Node_CallFunction* Condition = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//			Condition->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, Less_IntInt)));
//			Condition->AllocateDefaultPins();
//
//			//Connections to set up the loop
//			UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(Schema);
//			bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, IteratorInitialize->GetExecPin());
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorVar->GetVariablePin(), IteratorInitialize->GetVariablePin());
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorInitialize->GetThenPin(), Branch->GetExecPin());
//			bIsErrorFree &= Schema->TryCreateConnection(SpawnedActorReturnPin, ArrayLength->GetTargetArrayPin());
//			bIsErrorFree &= Schema->TryCreateConnection(Condition->GetReturnValuePin(), Branch->GetConditionPin());
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorVar->GetVariablePin(), Condition->FindPinChecked(TEXT("A")));
//			bIsErrorFree &= Schema->TryCreateConnection(ArrayLength->FindPin(K2Schema->PN_ReturnValue), Condition->FindPinChecked(TEXT("B")));
//
//			//Connections to establish loop iteration
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorVar->GetVariablePin(), Increment->FindPinChecked(TEXT("A")));
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorVar->GetVariablePin(), IteratorAssign->GetVariablePin());
//			bIsErrorFree &= Schema->TryCreateConnection(Increment->GetReturnValuePin(), IteratorAssign->GetValuePin());
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorAssign->GetThenPin(), Branch->GetExecPin());
//
//			//This is the inner loop
//			LastThenPin = Branch->GetThenPin();		//Connect the loop branch to the spawn-assignment code block
//			bIsErrorFree &= Schema->TryCreateConnection(SpawnedActorReturnPin, GetElement->GetTargetArrayPin());
//			bIsErrorFree &= Schema->TryCreateConnection(IteratorVar->GetVariablePin(), GetElement->FindPinChecked(K2Schema->PN_Index));
//			bIsErrorFree &= ConnectSpawnProperties(ClassToSpawn, Schema, CompilerContext, SourceGraph, LastThenPin, GetElement->FindPinChecked(K2Schema->PN_Item));		//Last argument is the array element
//			bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, IteratorAssign->GetExecPin());		//Connect the spawn-assignment code block to the iterator increment
//
//																										//Finish by providing the proper path out
//			LastThenPin = Branch->GetElsePin();
//		}
//
//		// -------------------------------------------
//		// Set spawn variables
//		//  Borrowed heavily from FKismetCompilerUtilities::GenerateAssignmentNodes
//		// -------------------------------------------
//
//		if (bValidatedActorSpawn && ClassToSpawn)
//		{
//			bIsErrorFree &= ConnectSpawnProperties(ClassToSpawn, Schema, CompilerContext, SourceGraph, LastThenPin, SpawnedActorReturnPin);
//		}
//
//		// -------------------------------------------
//		// Call FinishSpawning
//		// -------------------------------------------
//
//		UK2Node_CallFunction* const CallPostSpawnnProxyObjectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//		CallPostSpawnnProxyObjectNode->FunctionReference.SetExternalMember(ProxyPostpawnFunctionName, ProxyClass);
//		CallPostSpawnnProxyObjectNode->AllocateDefaultPins();
//
//		// Hook up the self connection
//		UEdGraphPin* PostspawnCallSelfPin = Schema->FindSelfPin(*CallPostSpawnnProxyObjectNode, EGPD_Input);
//		check(PostspawnCallSelfPin);
//
//		bIsErrorFree &= Schema->TryCreateConnection(ProxyObjectPin, PostspawnCallSelfPin);
//
//		// Link our Postspawn call in
//		bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, CallPostSpawnnProxyObjectNode->FindPinChecked(Schema->PN_Execute));
//
//		// Hook up any other input parameters to PostSpawn
//		for (auto CurrentPin : Pins)
//		{
//			if (FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Input, Schema))
//			{
//				UEdGraphPin* DestPin = CallPostSpawnnProxyObjectNode->FindPin(CurrentPin->PinName);
//				if (DestPin)
//				{
//					bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*CurrentPin, *DestPin).CanSafeConnect();
//				}
//			}
//		}
//
//
//		UEdGraphPin* InSpawnedActorPin = CallPostSpawnnProxyObjectNode->FindPin(TEXT("SpawnedActor"));
//		if (InSpawnedActorPin == nullptr)
//		{
//			CompilerContext.MessageLog.Error(*LOCTEXT("MissingSpawnedActorInputPin", "AbilityTask: Proxy is missing SpawnedActor input pin in FinishSpawningActor. @@").ToString(), this);
//			return;
//		}
//
//		bIsErrorFree &= Schema->TryCreateConnection(SpawnedActorReturnPin, InSpawnedActorPin);
//
//		LastThenPin = CallPostSpawnnProxyObjectNode->FindPinChecked(Schema->PN_Then);
//
//
//		// Move the connections from the original node then pin to the last internal then pin
//		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(Schema->PN_Then), *LastThenPin).CanSafeConnect();
//		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*LastThenPin, *BranchElsePin).CanSafeConnect();
//
//		if (!bIsErrorFree)
//		{
//			CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "BaseAsyncTask: Internal connection error. @@").ToString(), this);
//		}
//	//}
//	//else if (IsActor && IsActor->GetDefaultObject()->IsA(UGASAbilityMod::StaticClass()))
//	//{
//	//	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
//	//	UFunction* PreSpawnFunction = ProxyFactoryClass->FindFunctionByName(TEXT("SpawnObject"));
//	//	CallCreateNode->FunctionReference.SetExternalMember(TEXT("SpawnObject"), ProxyFactoryClass);
//	//	CallCreateNode->AllocateDefaultPins();
//	//	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, Schema->PC_Object, TEXT(""), UGASAbilityMod::StaticClass(), false, false, Schema->PN_ReturnValue);
//	//	//SetPinToolTip(*ResultPin, LOCTEXT("ResultPinDescription", "The spawned object"));
//
//	//	bool bSucceeded = true;
//	//	//connect exe
//	//	{
//	//		auto SpawnExecPin = GetExecPin();
//	//		auto CallExecPin = CallCreateNode->GetExecPin();
//	//		bSucceeded &= SpawnExecPin && CallExecPin && CompilerContext.MovePinLinksToIntermediate(*SpawnExecPin, *CallExecPin).CanSafeConnect();
//	//	}
//
//	//	//connect class
//	//	{
//	//		auto SpawnClassPin = FindPinChecked("InClass"); //GetClassPin();
//	//		auto CallClassPin = CallCreateNode->FindPin(TEXT("InClass"));
//	//		bSucceeded &= SpawnClassPin && CallClassPin && CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallClassPin).CanSafeConnect();
//	//	}
//
//	//	//connect outer
//	//	{
//	//		auto SpawnOuterPin = FindPinChecked("Outer"); //GetOuterPin();
//	//		auto CallOuterPin = CallCreateNode->FindPin(TEXT("Outer"));
//	//		bSucceeded &= SpawnOuterPin && CallOuterPin && CompilerContext.MovePinLinksToIntermediate(*SpawnOuterPin, *CallOuterPin).CanSafeConnect();
//	//	}
//
//	//	UEdGraphPin* CallResultPin = nullptr;
//	//	//connect result
//	//	{
//	//		auto SpawnResultPin = FindPinChecked(Schema->PN_ReturnValue); // GetResultPin();
//	//		CallResultPin = CallCreateNode->GetReturnValuePin();
//
//	//		// cast HACK. It should be safe. The only problem is native code generation.
//	//		if (SpawnResultPin && CallResultPin)
//	//		{
//	//			CallResultPin->PinType = SpawnResultPin->PinType;
//	//		}
//	//		bSucceeded &= SpawnResultPin && CallResultPin && CompilerContext.MovePinLinksToIntermediate(*SpawnResultPin, *CallResultPin).CanSafeConnect();
//	//	}
//
//	//	//assign exposed values and connect then
//	//	{
//	//		//auto LastThen = FindPinChecked(Schema->PN_Then);
//	//		auto LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, this, CallResultPin, GetClassToSpawn());
//	//		auto SpawnNodeThen = FindPinChecked(Schema->PN_Then); //GetThenPin();
//	//		bSucceeded &= SpawnNodeThen && LastThen && CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen).CanSafeConnect();
//	//	}
//	//}
//	// Make sure we caught everything
//	BreakAllNodeLinks();
//}

#undef LOCTEXT_NAMESPACE
